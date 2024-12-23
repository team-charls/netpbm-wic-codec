// SPDX-FileCopyrightText: © 2021 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "intellisense.hpp"

module netpbm_bitmap_frame_decode;

import std;
import winrt;
import <win.hpp>;

import hresults;
import buffered_stream_reader;
import pnm_header;
import util;
import "macros.hpp";

using std::int32_t;
using std::span;
using std::uint16_t;
using std::uint32_t;
using std::byteswap;
using std::ranges::transform;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::throw_hresult;

namespace {

[[nodiscard]] std::pair<GUID, uint32_t> get_pixel_format_and_shift(const PnmType type, const uint32_t bits_per_sample)
{
    switch (type)
    {
    case PnmType::Bitmap:
        break;

    case PnmType::Graymap:
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
        break;

    case PnmType::Pixmap:
        switch (bits_per_sample)
        {
        case 8:
            return {GUID_WICPixelFormat24bppRGB, 0};
        case 16:
            return {GUID_WICPixelFormat48bppRGB, 0};
        default:
            break;
        }
        break;
    }

    throw_hresult(wincodec::error_unsupported_pixel_format);
}

constexpr void convert_to_little_endian(span<uint16_t> samples) noexcept
{
    transform(samples, samples.begin(), [](const uint16_t sample) noexcept -> uint16_t { return byteswap(sample); });
}

constexpr void convert_to_little_endian_and_shift(span<uint16_t> samples, const uint32_t sample_shift) noexcept
{
    transform(samples, samples.begin(), [sample_shift](const uint16_t sample) noexcept -> uint16_t {
        return byteswap(sample) << sample_shift;
    });
}

constexpr void convert_to_little_endian_and_optional_shift(const span<uint16_t> samples,
                                                           const uint32_t sample_shift) noexcept
{
    if (sample_shift == 0)
    {
        convert_to_little_endian(samples);
    }
    else
    {
        convert_to_little_endian_and_shift(samples, sample_shift);
    }
}

void pack_to_crumbs(const span<const std::byte> byte_pixels, std::byte* crumb_pixels, const size_t width,
                    const size_t height, const size_t stride) noexcept
{
    for (size_t j{}, row{}; row != height; ++row)
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
        size_t i{};
        for (; i != width / 2; ++i)
        {
            nibble_row[i] = byte_pixels[j++] << 4;
            nibble_row[i] |= byte_pixels[j++];
        }
        if (width % 2)
        {
            nibble_row[i] = byte_pixels[j++] << 4;
        }
    }
}

void pack_to_bytes(const span<const std::byte> source_pixels, std::byte* destination_pixels, const size_t width,
                   const size_t height, const size_t stride) noexcept
{
    for (size_t row{}; row != height; ++row)
    {
        const std::byte* source_row{source_pixels.data() + row * width};
        std::byte* destination_row{destination_pixels + row * stride};
        std::copy_n(source_row, width, destination_row);
    }
}

void pack_to_words(const span<const uint16_t> source_pixels, uint16_t* destination_pixels, const size_t width,
                   const size_t height, const size_t stride) noexcept
{
    for (size_t row{}; row != height; ++row)
    {
        const uint16_t* source_row{source_pixels.data() + row * width};
        uint16_t* destination_row{destination_pixels + row * (stride / 2)};
        std::copy_n(source_row, width, destination_row);
    }
}

void decode_monochrome_bitmap(buffered_stream_reader& stream_reader, const pnm_header& header,
                              const uint32_t bits_per_sample, const uint32_t sample_shift, const uint32_t stride,
                              span<std::byte> destination_pixels)
{
    switch (bits_per_sample)
    {
    case 2:
        pack_to_crumbs(stream_reader.read_bytes(static_cast<size_t>(header.width) * header.height),
                       destination_pixels.data(), header.width, header.height, stride);
        break;

    case 4:
        pack_to_nibbles(stream_reader.read_bytes(static_cast<size_t>(header.width) * header.height),
                        destination_pixels.data(), header.width, header.height, stride);
        break;

    case 8:
        if (header.width % stride == 0)
        {
            stream_reader.read_bytes(destination_pixels.data(), destination_pixels.size());
        }
        else
        {
            pack_to_bytes(stream_reader.read_bytes(static_cast<size_t>(header.width) * header.height),
                          destination_pixels.data(), header.width, header.height, stride);
        }
        break;

    default:
        if (const auto header_width_in_bytes{static_cast<size_t>(header.width) * 2}; header_width_in_bytes % stride == 0)
        {
            // Binary 16 bit Netpbm images are stored in big endian format (the de facto standard).
            stream_reader.read_bytes(destination_pixels.data(), destination_pixels.size());
            convert_to_little_endian_and_optional_shift(
                {reinterpret_cast<uint16_t*>(destination_pixels.data()), destination_pixels.size() / sizeof uint16_t},
                sample_shift);
        }
        else
        {
            auto samples{stream_reader.read_bytes(header_width_in_bytes * header.height)};
            const span samples_16_bit{reinterpret_cast<uint16_t*>(samples.data()), samples.size() / sizeof uint16_t};
            convert_to_little_endian_and_optional_shift(samples_16_bit, sample_shift);
            pack_to_words(samples_16_bit, reinterpret_cast<uint16_t*>(destination_pixels.data()), header.width,
                          header.height, stride);
        }
        break;
    }
}

void decode_color_bitmap(buffered_stream_reader& stream_reader, const pnm_header& header, const uint32_t bits_per_sample,
                         const uint32_t stride, span<std::byte> destination_samples)
{
    constexpr size_t sample_per_pixel{3};

    switch (bits_per_sample)
    {
    case 8:
        if (const auto header_width_in_bytes{header.width * sample_per_pixel}; header_width_in_bytes % stride == 0)
        {
            stream_reader.read_bytes(destination_samples.data(), destination_samples.size());
        }
        else
        {
            std::byte* line{destination_samples.data()};
            for (uint32_t row{header.height}; row; --row)
            {
                stream_reader.read_bytes(line, header.width * size_t{3});
                line += stride;
            }
        }
        break;

    case 16: {
        constexpr size_t bytes_per_sample{2};
        if (const auto header_width_in_bytes{header.width * sample_per_pixel * bytes_per_sample};
            header_width_in_bytes % stride == 0)
        {
            stream_reader.read_bytes(destination_samples.data(), destination_samples.size());
        }
        else
        {
            std::byte* line{destination_samples.data()};
            for (uint32_t row{header.height}; row; --row)
            {
                stream_reader.read_bytes(line, header.width * sizeof uint16_t * 3);
                line += stride;
            }
        }
        convert_to_little_endian_and_optional_shift(
            {reinterpret_cast<uint16_t*>(destination_samples.data()), destination_samples.size() / sizeof uint16_t}, 0);
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}

[[nodiscard]] com_ptr<IWICBitmap> create_bitmap(_In_ IStream* source_stream, _In_ IWICImagingFactory* factory)
{
    buffered_stream_reader stream_reader{source_stream};
    const pnm_header header{stream_reader};
    const uint32_t bits_per_sample{static_cast<uint32_t>(std::bit_width(header.MaxColorValue))};
    const auto& [pixel_format, sample_shift] = get_pixel_format_and_shift(header.PnmType, bits_per_sample);

    com_ptr<IWICBitmap> bitmap;
    check_hresult(factory->CreateBitmap(header.width, header.height, pixel_format, WICBitmapCacheOnLoad, bitmap.put()));
    check_hresult(bitmap->SetResolution(96., 96.));

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

    switch (header.PnmType)
    {
    case PnmType::Graymap:
        decode_monochrome_bitmap(stream_reader, header, bits_per_sample, sample_shift, stride,
                                 {data_buffer, data_buffer_size});
        break;

    case PnmType::Pixmap:
        decode_color_bitmap(stream_reader, header, bits_per_sample, stride, {data_buffer, data_buffer_size});
        break;

    default:
        break;
    }

    return bitmap;
}

} // namespace


netpbm_bitmap_frame_decode::netpbm_bitmap_frame_decode(_In_ IStream* source_stream, _In_ IWICImagingFactory* factory) :
    bitmap_source_{create_bitmap(source_stream, factory)}
{
}


// IWICBitmapSource
HRESULT __stdcall netpbm_bitmap_frame_decode::GetSize(uint32_t* width, uint32_t* height)
{
    TRACE("{} netpbm_bitmap_frame_decode::GetSize, width address={}, height address={}\n", fmt_ptr(this), fmt_ptr(width),
          fmt_ptr(height));
    return bitmap_source_->GetSize(width, height);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::GetPixelFormat(GUID* pixel_format)
{
    TRACE("{} netpbm_bitmap_frame_decode::GetPixelFormat.1, pixel_format address={}\n", fmt_ptr(this),
          fmt_ptr(pixel_format));
    return bitmap_source_->GetPixelFormat(pixel_format);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::GetResolution(double* dpi_x, double* dpi_y)
{
    TRACE("{} netpbm_bitmap_frame_decode::GetResolution, dpi_x address={}, dpi_y address={}\n", fmt_ptr(this),
          fmt_ptr(dpi_x), fmt_ptr(dpi_y));
    return bitmap_source_->GetResolution(dpi_x, dpi_y);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::CopyPixels(const WICRect* rectangle, const uint32_t stride,
                                                         const uint32_t buffer_size, BYTE* buffer)
{
    TRACE("{} netpbm_bitmap_frame_decode::CopyPixels, rectangle address={}, stride={}, buffer_size={}, buffer "
          "address={}\n",
          fmt_ptr(this), static_cast<const void*>(rectangle), stride, buffer_size, fmt_ptr(buffer));
    return bitmap_source_->CopyPixels(rectangle, stride, buffer_size, buffer);
}

HRESULT __stdcall netpbm_bitmap_frame_decode::CopyPalette(IWICPalette*) noexcept
{
    TRACE("{} netpbm_bitmap_frame_decode::CopyPalette (not supported)\n", fmt_ptr(this));
    return wincodec::error_palette_unavailable;
}

// IWICBitmapFrameDecode : IWICBitmapSource

HRESULT __stdcall netpbm_bitmap_frame_decode::GetThumbnail(IWICBitmapSource**) noexcept
{
    TRACE("{} netpbm_bitmap_frame_decode::GetThumbnail (not supported)\n", fmt_ptr(this));
    return wincodec::error_codec_no_thumbnail;
}

HRESULT __stdcall netpbm_bitmap_frame_decode::GetColorContexts(const uint32_t count, IWICColorContext** color_contexts,
                                                               uint32_t* actual_count) noexcept
try
{
    TRACE("{} netpbm_bitmap_frame_decode::GetColorContexts (always 0), count={}, color_contexts address={}, "
          "actual_count address={}\n",
          fmt_ptr(this), count, fmt_ptr(color_contexts), fmt_ptr(actual_count));
    *check_out_pointer(actual_count) = 0;
    return success_ok;
}
catch (...)
{
    return to_hresult();
}

HRESULT __stdcall netpbm_bitmap_frame_decode::GetMetadataQueryReader(
    [[maybe_unused]] IWICMetadataQueryReader** metadata_query_reader) noexcept
{
    TRACE("{} netpbm_bitmap_decoder::GetMetadataQueryReader, metadata_query_reader address={}\n", fmt_ptr(this),
          fmt_ptr(metadata_query_reader));
    return wincodec::error_unsupported_operation;
}
