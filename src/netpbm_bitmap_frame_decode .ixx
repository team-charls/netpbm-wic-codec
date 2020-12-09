// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

#include <winerror.h>
#include <winrt/base.h>

#include "trace.h"
#include "util.h"

export module netpbm_bitmap_frame_decode;

import errors;
import buffered_stream_reader;
import pnm_header;

using winrt::to_hresult;
using winrt::check_hresult;
using winrt::com_ptr;

export struct netpbm_bitmap_frame_decode final : winrt::implements<netpbm_bitmap_frame_decode, IWICBitmapFrameDecode, IWICBitmapSource>
{
    netpbm_bitmap_frame_decode(buffered_stream_reader& stream_reader, IWICImagingFactory* factory) :
        stream_reader_{stream_reader},
        header_{stream_reader_}
    {

        auto pixel_format_info = get_pixel_format(8);
        if (!pixel_format_info)
            throw_hresult(wincodec::error_unsupported_pixel_format);

        const auto& [pixel_format, sample_shift] = pixel_format_info.value();
        com_ptr<IWICBitmap> bitmap;
        check_hresult(factory->CreateBitmap(header_.width, header_.height, pixel_format, WICBitmapCacheOnLoad, bitmap.put()));
        check_hresult(bitmap->SetResolution(96, 96));

        {
            WICRect complete_image{0, 0, static_cast<int32_t>(header_.width), static_cast<int32_t>(header_.height)};
            com_ptr<IWICBitmapLock> bitmap_lock;
            check_hresult(bitmap->Lock(&complete_image, WICBitmapLockWrite, bitmap_lock.put()));

            uint32_t stride;
            winrt::check_hresult(bitmap_lock->GetStride(&stride));

            BYTE* data_buffer;
            uint32_t data_buffer_size;
            winrt::check_hresult(bitmap_lock->GetDataPointer(&data_buffer_size, &data_buffer));
            __assume(data_buffer != nullptr);

            ////decoder.decode(data_buffer, data_buffer_size, stride);

            if (sample_shift != 0)
            {
                ////shift_samples(data_buffer, data_buffer_size / 2, sample_shift);
            }
        }

        check_hresult(bitmap->QueryInterface(bitmap_source_.put()));
    }

    // IWICBitmapSource
    HRESULT __stdcall GetSize(uint32_t* width, uint32_t* height) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetSize, width=%p, height=%p\n", this, width, height);

        WARNING_SUPPRESS_NEXT_LINE(26447) // noexcept: COM methods are not defined as noexcept
        return bitmap_source_->GetSize(width, height);
    }

    HRESULT __stdcall GetPixelFormat(GUID* pixel_format) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetPixelFormat.1, pixel_format=%p\n", this, pixel_format);

        WARNING_SUPPRESS_NEXT_LINE(26447) // noexcept: COM methods are not defined as noexcept
        return bitmap_source_->GetPixelFormat(pixel_format);
    }

    HRESULT __stdcall GetResolution(double* dpi_x, double* dpi_y) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetResolution, dpi_x=%p, dpi_y=%p\n", this, dpi_x, dpi_y);

        WARNING_SUPPRESS_NEXT_LINE(26447) // noexcept: COM methods are not defined as noexcept
        return bitmap_source_->GetResolution(dpi_x, dpi_y);
    }

    HRESULT __stdcall CopyPixels(const WICRect* rectangle, const uint32_t stride, const uint32_t buffer_size, BYTE* buffer) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::CopyPixels, rectangle=%p, buffer_size=%d, buffer=%p\n", this, rectangle, buffer_size, buffer);

        WARNING_SUPPRESS_NEXT_LINE(26447) // noexcept: COM methods are not defined as noexcept
        return bitmap_source_->CopyPixels(rectangle, stride, buffer_size, buffer);
    }

    HRESULT __stdcall CopyPalette(IWICPalette*) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::CopyPalette\n", this);
        return wincodec::error_palette_unavailable;
    }

    // IWICBitmapFrameDecode : IWICBitmapSource
    HRESULT __stdcall GetThumbnail(IWICBitmapSource**) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetThumbnail\n", this);
        return wincodec::error_codec_no_thumbnail;
    }

    HRESULT __stdcall GetColorContexts(const uint32_t count, IWICColorContext** color_contexts, uint32_t* actual_count) noexcept override
    {
        TRACE("%p netpbm_bitmap_frame_decode::GetColorContexts, count=%d, color_contexts=%p, actual_count=%p\n", this, count, color_contexts, actual_count);
        *check_out_pointer(actual_count) = 0;
        return error_ok;
    }

    HRESULT __stdcall GetMetadataQueryReader([[maybe_unused]] IWICMetadataQueryReader** metadata_query_reader) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetMetadataQueryReader, metadata_query_reader=%p\n", this, metadata_query_reader);
        return wincodec::error_unsupported_operation;
    }

private:
    static std::optional<std::pair<GUID, uint32_t>> get_pixel_format(const int32_t bits_per_sample) noexcept
    {
        switch (bits_per_sample)
        {
        case 8:
            return std::make_pair(GUID_WICPixelFormat8bppGray, 0);

        case 10:
        case 12:
        case 16:
            return std::make_pair(GUID_WICPixelFormat16bppGray, 16 - bits_per_sample);

        default:
            break;
        }

        return {};
    }

    buffered_stream_reader& stream_reader_;
    pnm_header header_;
    winrt::com_ptr<IWICBitmapSource> bitmap_source_;
};
