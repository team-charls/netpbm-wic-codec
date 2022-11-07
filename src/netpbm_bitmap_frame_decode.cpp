// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "macros.h"
#include "winrt.h"
#include "wic_bitmap_source.h"

module netpbm_bitmap_frame_decode;

import errors;
import buffered_stream_reader;
import pnm_header;
import trace;
import <algorithm>;
import <bit>;
import <span>;
import <wincodec.h>;

using std::span;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::throw_hresult;
using winrt::to_hresult;


namespace {

std::pair<GUID, uint32_t> get_pixel_format_and_shift(const uint32_t bits_per_sample)
{
    switch (bits_per_sample)
    {
    case 2:
        return {GUID_WICPixelFormat2bppGray, 0};

    case 4:
        return {GUID_WICPixelFormat4bppGray, 0};

    case 8:
        return {GUID_WICPixelFormat8bppGray, 0};

    case 10:
    case 12:
    case 16:
        return {GUID_WICPixelFormat16bppGray, 16 - bits_per_sample};

    default:
        break;
    }

    throw_hresult(wincodec::error_unsupported_pixel_format);
}

constexpr void convert_to_little_endian(span<uint16_t> samples) noexcept
{
    std::ranges::transform(samples, samples.begin(),
                           [](const uint16_t sample) noexcept -> uint16_t { return _byteswap_ushort(sample); });
}

constexpr void convert_to_little_endian_and_shift(span<uint16_t> samples, const uint32_t sample_shift) noexcept
{
    std::ranges::transform(samples, samples.begin(), [sample_shift](const uint16_t sample) noexcept -> uint16_t {
        return _byteswap_ushort(sample) << sample_shift;
    });
}

void pack_to_crumbs(const span<const std::byte> byte_pixels, std::byte* crumb_pixels, const size_t width,
                    const size_t height, const size_t stride) noexcept
{
    size_t j{};
    for (size_t row{}; row != height; ++row)
    {
        std::byte* crumb_row{crumb_pixels + (row * stride)};
        size_t i{};
        for (; i != width / 4; ++i)
        {
            std::byte value{byte_pixels[j++] << 6};
            value |= byte_pixels[j++] << 4;
            value |= byte_pixels[j++] << 2;
            value |= byte_pixels[j++];
            crumb_row[i] = value;
        }

        switch (width % 4)
        {
        case 3:
            crumb_row[i] = byte_pixels[j++] << 6;
            [[fallthrough]];

        case 2:
            crumb_row[i] |= byte_pixels[j++] << 4;
            [[fallthrough]];

        case 1:
            crumb_row[i] |= byte_pixels[j++] << 2;
            break;

        default:
            break;
        }
    }
}

void pack_to_nibbles(const span<const std::byte> byte_pixels, std::byte* nibble_pixels, const size_t width,
                     const size_t height, const size_t stride) noexcept
{
    for (size_t j{}, row{}; row != height; ++row)
    {
        std::byte* nibble_row{nibble_pixels + (row * stride)};
        for (size_t i{}; i != width / 2; ++i)
        {
            nibble_row[i] = byte_pixels[j++] << 4;
            nibble_row[i] |= byte_pixels[j++];
        }
    }
}

com_ptr<IWICBitmap> create_bitmap(_In_ IStream* source_stream, _In_ IWICImagingFactory* factory)
{
    buffered_stream_reader stream_reader{source_stream};
    const pnm_header header{stream_reader};

    const uint32_t bits_per_sample{static_cast<uint32_t>(std::bit_width(header.MaxColorValue))};
    const auto& [pixel_format, sample_shift] = get_pixel_format_and_shift(bits_per_sample);
    com_ptr<IWICBitmap> bitmap;
    check_hresult(factory->CreateBitmap(header.width, header.height, pixel_format, WICBitmapCacheOnLoad, bitmap.put()));
    check_hresult(bitmap->SetResolution(96., 96.));

    {
        const WICRect complete_image{
            .X{0}, .Y{0}, .Width{static_cast<int32_t>(header.width)}, .Height{static_cast<int32_t>(header.height)}};
        com_ptr<IWICBitmapLock> bitmap_lock;
        check_hresult(bitmap->Lock(&complete_image, WICBitmapLockWrite, bitmap_lock.put()));

        uint32_t stride;
        winrt::check_hresult(bitmap_lock->GetStride(&stride));

        std::byte* data_buffer;
        uint32_t data_buffer_size;
        winrt::check_hresult(bitmap_lock->GetDataPointer(&data_buffer_size, reinterpret_cast<BYTE**>(&data_buffer)));
        __assume(data_buffer != nullptr);

        switch (bits_per_sample)
        {
        case 2:
            pack_to_crumbs(stream_reader.read_bytes(static_cast<size_t>(header.width) * header.height), data_buffer,
                           header.width, header.height, stride);
            break;

        case 4:
            pack_to_nibbles(stream_reader.read_bytes(static_cast<size_t>(header.width) * header.height), data_buffer,
                            header.width, header.height, stride);
            break;

        case 8:
            stream_reader.read_bytes(data_buffer, data_buffer_size);
            break;

        default:
            // Binary 16 bit Netpbm images are stored in big endian format (the defacto standard).
            stream_reader.read_bytes(data_buffer, data_buffer_size);
            const span samples_16_bit{reinterpret_cast<uint16_t*>(data_buffer), data_buffer_size / sizeof uint16_t};

            if (sample_shift == 0)
            {
                convert_to_little_endian(samples_16_bit);
            }
            else
            {
                convert_to_little_endian_and_shift(samples_16_bit, sample_shift);
            }
            break;
        }
    }

    return bitmap;
}

} // namespace


netpbm_bitmap_frame_decode::netpbm_bitmap_frame_decode(_In_ IStream* source_stream, _In_ IWICImagingFactory* factory) :
    bitmap_source_{create_bitmap(source_stream, factory)}
{
}


// IWICBitmapSource
HRESULT __stdcall netpbm_bitmap_frame_decode::GetSize(uint32_t* width, uint32_t* height) noexcept
{
    TRACE("%p netpbm_bitmap_frame_decode::GetSize, width address=%p, height address=%p\n", this, width, height);
    return bitmap_source_->GetSize(width, height);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::GetPixelFormat(GUID* pixel_format) noexcept
{
    TRACE("%p netpbm_bitmap_frame_decode::GetPixelFormat.1, pixel_format address=%p\n", this, pixel_format);
    return bitmap_source_->GetPixelFormat(pixel_format);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::GetResolution(double* dpi_x, double* dpi_y) noexcept
{
    TRACE("%p netpbm_bitmap_frame_decode::GetResolution, dpi_x address=%p, dpi_y address=%p\n", this, dpi_x, dpi_y);
    return bitmap_source_->GetResolution(dpi_x, dpi_y);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::CopyPixels(const WICRect* rectangle, const uint32_t stride,
                                                         const uint32_t buffer_size, BYTE* buffer) noexcept
{
    TRACE("%p netpbm_bitmap_frame_decode::CopyPixels, rectangle address=%p, stride=%d, buffer_size=%d, buffer "
          "address=%p\n",
          this, rectangle, stride, buffer_size, buffer);
    return bitmap_source_->CopyPixels(rectangle, stride, buffer_size, buffer);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::CopyPalette(IWICPalette*) noexcept
{
    TRACE("%p netpbm_bitmap_frame_decode::CopyPalette (not supported)\n", this);
    return wincodec::error_palette_unavailable;
}

// IWICBitmapFrameDecode : IWICBitmapSource
SUPPRESS_WARNING_NEXT_LINE(6101)
HRESULT __stdcall netpbm_bitmap_frame_decode::GetThumbnail(IWICBitmapSource**) noexcept
{
    TRACE("%p netpbm_bitmap_frame_decode::GetThumbnail (not supported)\n", this);
    return wincodec::error_codec_no_thumbnail;
}

HRESULT __stdcall netpbm_bitmap_frame_decode::GetColorContexts(const uint32_t count, IWICColorContext** color_contexts,
                                                               uint32_t* actual_count) noexcept
try
{
    TRACE("%p netpbm_bitmap_frame_decode::GetColorContexts (always 0), count=%d, color_contexts address=%p, "
          "actual_count address=%p\n",
          this, count, color_contexts, actual_count);
    *check_out_pointer(actual_count) = 0;
    return error_ok;
}
catch (...)
{
    return to_hresult();
}

SUPPRESS_WARNING_NEXT_LINE(6101)
HRESULT __stdcall netpbm_bitmap_frame_decode::GetMetadataQueryReader(
    [[maybe_unused]] IWICMetadataQueryReader** metadata_query_reader) noexcept
{
    TRACE("%p netpbm_bitmap_decoder::GetMetadataQueryReader, metadata_query_reader address=%p\n", this,
          metadata_query_reader);
    return wincodec::error_unsupported_operation;
}
