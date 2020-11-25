// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

#include <winerror.h>
#include <winrt/base.h>

export module netpbm_bitmap_frame_decode_;


#ifdef NDEBUG

#define TRACE __noop

#else

template<typename... Args>
void trace(char const* const message, Args... args) noexcept
{
    char buffer[1024];
    static_cast<void>(snprintf(buffer, sizeof buffer, message, args...)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    OutputDebugStringA(buffer);
}

#define TRACE trace

#endif


inline constexpr winrt::hresult error_ok{S_OK};
inline constexpr winrt::hresult error_fail{E_FAIL};



export struct netpbm_bitmap_frame_decode final : winrt::implements<netpbm_bitmap_frame_decode, IWICBitmapFrameDecode, IWICBitmapSource>
{
    netpbm_bitmap_frame_decode(IStream* stream, IWICImagingFactory* factory)
    {
    }

    // IWICBitmapSource
    HRESULT __stdcall GetSize(uint32_t* width, uint32_t* height) noexcept override
    {
        TRACE("%p jpegls_bitmap_frame_decoder::GetSize.1, width=%p, height=%p\n", this, width, height);
        return error_fail;
    }

    HRESULT __stdcall GetPixelFormat(GUID* pixel_format) noexcept override
    {
        TRACE("%p jpegls_bitmap_frame_decoder::GetPixelFormat.1, pixel_format=%p\n", this, pixel_format);
        return error_fail;
    }

    HRESULT __stdcall GetResolution(double* dpi_x, double* dpi_y) noexcept override
    {
        TRACE("%p jpegls_bitmap_frame_decoder::GetResolution, dpi_x=%p, dpi_y=%p\n", this, dpi_x, dpi_y);
        return error_fail;
    }

    HRESULT __stdcall CopyPixels(const WICRect* rectangle, const uint32_t stride, const uint32_t buffer_size, BYTE* buffer) noexcept override
    {
        TRACE("%p jpegls_bitmap_frame_decoder::CopyPixels, rectangle=%p, buffer_size=%d, buffer=%p\n", this, rectangle, buffer_size, buffer);
        return error_fail;
    }

    HRESULT __stdcall CopyPalette(IWICPalette*) noexcept override
    {
        TRACE("%p jpegls_bitmap_frame_decoder::CopyPalette\n", this);
        return error_fail;
    }

    // IWICBitmapFrameDecode : IWICBitmapSource
    HRESULT __stdcall GetThumbnail(IWICBitmapSource**) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall GetColorContexts(const uint32_t count, IWICColorContext** color_contexts, uint32_t* actual_count) noexcept override
    {
        TRACE("%p jpegls_bitmap_frame_decoder::GetColorContexts, count=%d, color_contexts=%p, actual_count=%p\n", this, count, color_contexts, actual_count);
        return error_fail;
    }

    HRESULT __stdcall GetMetadataQueryReader([[maybe_unused]] IWICMetadataQueryReader** metadata_query_reader) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetMetadataQueryReader, metadata_query_reader=%p\n", this, metadata_query_reader);
        return error_fail;
    }

private:

    winrt::com_ptr<IWICBitmapSource> bitmap_source_;
};
