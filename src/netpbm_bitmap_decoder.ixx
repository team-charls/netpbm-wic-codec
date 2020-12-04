// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

#include "trace.h"

#include <winrt/base.h>

export module netpbm_bitmap_decoder;

import class_factory;
import errors;
import buffered_stream_reader;
import pnm_header;
import guids;

using winrt::check_hresult;
using winrt::com_ptr;
using winrt::implements;
using winrt::make;
using winrt::to_hresult;

// {70ab66f5-cd48-43a1-aa29-10131b7f4ff1}
constexpr GUID GUID_ContainerFormatNetPbm{0x70ab66f5, 0xcd48, 0x43a1, {0xaa, 0x29, 0x10, 0x13, 0x1b, 0x7f, 0x4f, 0xf1}};


struct netpbm_bitmap_decoder final : implements<netpbm_bitmap_decoder, IWICBitmapDecoder>
{
    // IWICBitmapDecoder
    HRESULT __stdcall QueryCapability(_In_ IStream* stream, _Out_ DWORD* capability) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::QueryCapability.1, stream=%p, capability=%p\n", this, stream, capability);

        if (!stream)
            return error_invalid_argument;

        if (!capability)
            return error_pointer;

        *capability = 0;

        // Custom decoder implementations should save the current position of the specified IStream,
        // read whatever information is necessary in order to determine which capabilities
        // it can provide for the supplied stream, and restore the stream position.
        try
        {
            ULARGE_INTEGER original_position;

            if (FAILED(stream->Seek({}, STREAM_SEEK_CUR, &original_position)))
                return wincodec::error_stream_not_available;

            buffered_stream_reader stream_reader(stream);
            if (is_pnm_file(stream_reader))
            {
                *capability = WICBitmapDecoderCapabilityCanDecodeAllImages;
            }

            check_hresult(stream->Seek(*reinterpret_cast<LARGE_INTEGER*>(&original_position), STREAM_SEEK_CUR, nullptr));
            TRACE("%p netpbm_bitmap_decoder::QueryCapability.2, stream=%p, *capability=%d\n", this, stream, *capability);
            return error_ok;
        }
        catch (...)
        {
            return to_hresult();
        }
    }

    HRESULT __stdcall Initialize(_In_ IStream* stream, [[maybe_unused]] const WICDecodeOptions cache_options) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::Initialize, stream=%p, cache_options=%d\n", this, stream, cache_options);
        return error_fail;
    }

    HRESULT __stdcall GetContainerFormat(_Out_ GUID* container_format) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetContainerFormat, container_format=%p\n", this, container_format);
        if (!container_format)
            return error_pointer;

        *container_format = GUID_ContainerFormatNetPbm;
        return error_ok;
    }

    HRESULT __stdcall GetDecoderInfo(_Outptr_ IWICBitmapDecoderInfo** decoder_info) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetContainerFormat, decoder_info=%p\n", this, decoder_info);

        try
        {
            com_ptr<IWICComponentInfo> component_info;
            check_hresult(imaging_factory()->CreateComponentInfo(CLSID_NetPbmDecoder, component_info.put()));
            check_hresult(component_info->QueryInterface(IID_PPV_ARGS(decoder_info)));

            return error_ok;
        }
        catch (...)
        {
            return to_hresult();
        }
    }

    HRESULT __stdcall CopyPalette([[maybe_unused]] _In_ IWICPalette* palette) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::CopyPalette, palette=%p\n", this, palette);

        // NetPbm images don;t have palettes.
        return wincodec::error_palette_unavailable;
    }

    HRESULT __stdcall GetMetadataQueryReader([[maybe_unused]] _Outptr_ IWICMetadataQueryReader** metadata_query_reader) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetMetadataQueryReader, metadata_query_reader=%p\n", this, metadata_query_reader);

        // Keep the initial design simple: no support for container-level metadata.
        // Note: Conceptual, comments from the NetPbm file could converted into metadata.
        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall GetPreview([[maybe_unused]] _Outptr_ IWICBitmapSource** bitmap_source) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetPreview, bitmap_source=%p\n", this, bitmap_source);
        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall GetColorContexts([[maybe_unused]] const uint32_t count, [[maybe_unused]] IWICColorContext** color_contexts, [[maybe_unused]] uint32_t* actual_count) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetColorContexts, count=%u, color_contexts=%p, actual_count=%p\n", this, count, color_contexts, actual_count);
        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall GetThumbnail([[maybe_unused]] _Outptr_ IWICBitmapSource** thumbnail) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetThumbnail, thumbnail=%p\n", this, thumbnail);
        return wincodec::error_codec_no_thumbnail;
    }

    HRESULT __stdcall GetFrameCount(_Out_ uint32_t* count) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetFrameCount, count=%p\n", this, count);
        if (!count)
            return error_pointer;

        *count = 1; // Only 1 frame is supported by this implementation (no real world sampe are known that have more)
        return error_ok;
    }

    HRESULT __stdcall GetFrame(const uint32_t index, _Outptr_ IWICBitmapFrameDecode** bitmap_frame_decode) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetFrame, index=%d, bitmap_frame_decode=%p\n", this, index, bitmap_frame_decode);
        return error_fail;
    }

private:
    IWICImagingFactory* imaging_factory()
    {
        if (!imaging_factory_)
        {
            check_hresult(CoCreateInstance(CLSID_WICImagingFactory,
                                           nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(imaging_factory_.put())));
        }

        return imaging_factory_.get();
    }

    com_ptr<IWICImagingFactory> imaging_factory_;
    com_ptr<IStream> stream_;
    com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode_;
};

export HRESULT create_netpbm_bitmap_decoder_factory(_In_ GUID const& interface_id, _Outptr_ void** result)
{
    return make<class_factory<netpbm_bitmap_decoder>>()->QueryInterface(interface_id, result);
}
