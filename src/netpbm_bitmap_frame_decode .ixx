// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

#include <winerror.h>
#include <winrt/base.h>

#include "macros.h"
#include "trace.h"

#include <bit>
#include <span>

export module netpbm_bitmap_frame_decode;

import errors;
import buffered_stream_reader;
import pnm_header;

using std::byte;
using std::make_pair;
using std::span;
using std::transform;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::to_hresult;


namespace {

std::pair<GUID, uint32_t> get_pixel_format_and_shift(const uint32_t bits_per_sample)
{
    switch (bits_per_sample)
    {
    case 8:
        return make_pair(GUID_WICPixelFormat8bppGray, 0);

    case 10:
    case 12:
    case 16:
        return make_pair(GUID_WICPixelFormat16bppGray, 16 - bits_per_sample);

    default:
        break;
    }

    throw_hresult(wincodec::error_unsupported_pixel_format);
}

void convert_to_little_endian(span<uint16_t> samples) noexcept
{
    transform(samples.begin(), samples.end(), samples.begin(),
              [](const uint16_t sample) -> uint16_t { return _byteswap_ushort(sample); });
}

void convert_to_little_endian_and_shift(span<uint16_t> samples, const uint32_t sample_shift) noexcept
{
    transform(samples.begin(), samples.end(), samples.begin(),
              [sample_shift](const uint16_t sample) -> uint16_t { return _byteswap_ushort(sample) << sample_shift; });
}

com_ptr<IWICBitmap> create_bitmap(_In_ IStream* source_stream, _In_ IWICImagingFactory* factory)
{
    buffered_stream_reader stream_reader{source_stream};
    pnm_header header{stream_reader};

    const uint32_t bits_per_sample{std::bit_width(header.MaxColorValue)};
    const auto& [pixel_format, sample_shift] = get_pixel_format_and_shift(bits_per_sample);
    com_ptr<IWICBitmap> bitmap;
    check_hresult(factory->CreateBitmap(header.width, header.height, pixel_format, WICBitmapCacheOnLoad, bitmap.put()));
    check_hresult(bitmap->SetResolution(96, 96));

    {
        const WICRect complete_image{0, 0, static_cast<int32_t>(header.width), static_cast<int32_t>(header.height)};
        com_ptr<IWICBitmapLock> bitmap_lock;
        check_hresult(bitmap->Lock(&complete_image, WICBitmapLockWrite, bitmap_lock.put()));

        uint32_t stride;
        winrt::check_hresult(bitmap_lock->GetStride(&stride));

        byte* data_buffer;
        uint32_t data_buffer_size;
        winrt::check_hresult(bitmap_lock->GetDataPointer(&data_buffer_size, reinterpret_cast<BYTE**>(&data_buffer)));
        __assume(data_buffer != nullptr);

        stream_reader.read_bytes(data_buffer, data_buffer_size);

        if (bits_per_sample > 8)
        {
            // Binary 16 bit Netpbm images are stored in big endian format (de facto standard).
            const span<uint16_t> samples_16bit{reinterpret_cast<uint16_t*>(data_buffer), data_buffer_size / sizeof uint16_t};

            if (sample_shift == 0)
            {
                convert_to_little_endian(samples_16bit);
            }
            else
            {
                convert_to_little_endian_and_shift(samples_16bit, sample_shift);
            }
        }
    }

    return bitmap;
}

} // namespace


export class netpbm_bitmap_frame_decode final
    : public winrt::implements<netpbm_bitmap_frame_decode, IWICBitmapFrameDecode, IWICBitmapSource>
{
public:
    netpbm_bitmap_frame_decode(_In_ IStream* source_stream, _In_ IWICImagingFactory* factory) :
        bitmap_source_{create_bitmap(source_stream, factory)}
    {
    }

    // IWICBitmapSource
    HRESULT __stdcall GetSize(uint32_t* width, uint32_t* height) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetSize, width address=%p, height address=%p\n", this, width, height);
        return bitmap_source_->GetSize(width, height);
    }

    HRESULT __stdcall GetPixelFormat(GUID* pixel_format) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetPixelFormat.1, pixel_format address=%p\n", this, pixel_format);
        return bitmap_source_->GetPixelFormat(pixel_format);
    }

    HRESULT __stdcall GetResolution(double* dpi_x, double* dpi_y) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetResolution, dpi_x address=%p, dpi_y address=%p\n", this, dpi_x, dpi_y);
        return bitmap_source_->GetResolution(dpi_x, dpi_y);
    }

    HRESULT __stdcall CopyPixels(const WICRect* rectangle, const uint32_t stride, const uint32_t buffer_size,
                                 BYTE* buffer) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::CopyPixels, rectangle address=%p, stride=%d, buffer_size=%d, buffer "
              "address=%p\n",
              this, rectangle, stride, buffer_size, buffer);
        return bitmap_source_->CopyPixels(rectangle, stride, buffer_size, buffer);
    }

    HRESULT __stdcall CopyPalette(IWICPalette*) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::CopyPalette (not supported)\n", this);
        return wincodec::error_palette_unavailable;
    }

    // IWICBitmapFrameDecode : IWICBitmapSource
    HRESULT __stdcall GetThumbnail(IWICBitmapSource**) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetThumbnail (not supported)\n", this);
        return wincodec::error_codec_no_thumbnail;
    }

    HRESULT __stdcall GetColorContexts(const uint32_t count, IWICColorContext** color_contexts,
                                       uint32_t* actual_count) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetColorContexts (always 0), count=%d, color_contexts address=%p, "
              "actual_count address=%p\n",
              this, count, color_contexts, actual_count);
        *check_out_pointer(actual_count) = 0;
        return error_ok;
    }

    HRESULT __stdcall GetMetadataQueryReader(
        [[maybe_unused]] IWICMetadataQueryReader** metadata_query_reader) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetMetadataQueryReader, metadata_query_reader address=%p\n", this,
              metadata_query_reader);
        return wincodec::error_unsupported_operation;
    }

private:
    com_ptr<IWICBitmapSource> bitmap_source_;
};
