// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "winrt.h"

export module netpbm_bitmap_frame_decode;

import <cstdint>;
import <wincodec.h>;

export class netpbm_bitmap_frame_decode final
    : public winrt::implements<netpbm_bitmap_frame_decode, IWICBitmapFrameDecode, IWICBitmapSource>
{
public:
    netpbm_bitmap_frame_decode(_In_ IStream* source_stream, _In_ IWICImagingFactory* factory);

    // IWICBitmapSource
    HRESULT __stdcall GetSize(uint32_t* width, uint32_t* height) override;
    HRESULT __stdcall GetPixelFormat(GUID* pixel_format) override;
    HRESULT __stdcall GetResolution(double* dpi_x, double* dpi_y) override;
    HRESULT __stdcall CopyPixels(const WICRect* rectangle, const uint32_t stride, const uint32_t buffer_size,
                                 BYTE* buffer) override;
    HRESULT __stdcall CopyPalette(IWICPalette*) noexcept override;

    // IWICBitmapFrameDecode : IWICBitmapSource
    HRESULT __stdcall GetThumbnail(IWICBitmapSource**) noexcept override;
    HRESULT __stdcall GetColorContexts(const uint32_t count, IWICColorContext** color_contexts,
                                       uint32_t* actual_count) override;
    HRESULT __stdcall GetMetadataQueryReader([[maybe_unused]] IWICMetadataQueryReader** metadata_query_reader)
        noexcept override;

private:
    winrt::com_ptr<IWICBitmapSource> bitmap_source_;
};

