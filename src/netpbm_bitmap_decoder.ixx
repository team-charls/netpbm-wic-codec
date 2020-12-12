// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

#include "trace.h"
#include "macros.h"

#include <winrt/base.h>

#include <mutex>

export module netpbm_bitmap_decoder;

import class_factory;
import errors;
import buffered_stream_reader;
import pnm_header;
import guids;
import netpbm_bitmap_frame_decode;

using winrt::check_hresult;
using winrt::com_ptr;
using winrt::to_hresult;
using std::scoped_lock;


struct netpbm_bitmap_decoder final : winrt::implements<netpbm_bitmap_decoder, IWICBitmapDecoder>
{
    // IWICBitmapDecoder
    HRESULT __stdcall QueryCapability(_In_ IStream* stream, _Out_ DWORD* capability) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::QueryCapability.1, stream=%p, capability=%p\n", this, stream, capability);

        check_in_pointer(stream);
        *check_out_pointer(capability) = 0;

        // Custom decoder implementations should save the current position of the specified IStream,
        // read whatever information is necessary in order to determine which capabilities
        // it can provide for the supplied stream, and restore the stream position.
        ULARGE_INTEGER original_position;
        check_hresult(stream->Seek({}, STREAM_SEEK_CUR, &original_position));

        if (is_pnm_file(stream))
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

    HRESULT __stdcall Initialize(_In_ IStream* stream, [[maybe_unused]] const WICDecodeOptions cache_options) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::Initialize, stream=%p, cache_options=%d\n", this, stream, cache_options);

        WARNING_SUPPRESS_NEXT_LINE(26447) // noexcept: false warning, caused by scoped_lock
        scoped_lock lock{mutex_};

        stream_reader_ = std::make_unique<buffered_stream_reader>(check_in_pointer(stream));
        bitmap_frame_decode_.attach(nullptr);

        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetContainerFormat(_Out_ GUID* container_format) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::GetContainerFormat, container_format=%p\n", this, container_format);

        *check_out_pointer(container_format) = GUID_ContainerFormatNetPbm;
        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetDecoderInfo(_Outptr_ IWICBitmapDecoderInfo** decoder_info) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::GetContainerFormat, decoder_info=%p\n", this, decoder_info);

        com_ptr<IWICComponentInfo> component_info;
        check_hresult(imaging_factory()->CreateComponentInfo(CLSID_NetPbmDecoder, component_info.put()));
        check_hresult(component_info->QueryInterface(IID_PPV_ARGS(decoder_info)));

        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
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
    try
    {
        TRACE("%p netpbm_bitmap_decoder::GetColorContexts, count=%u, color_contexts=%p, actual_count=%p\n", this, count, color_contexts, actual_count);

        *check_out_pointer(actual_count) = 0;
        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetThumbnail([[maybe_unused]] _Outptr_ IWICBitmapSource** thumbnail) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetThumbnail, thumbnail=%p\n", this, thumbnail);
        return wincodec::error_codec_no_thumbnail;
    }

    HRESULT __stdcall GetFrameCount(_Out_ uint32_t* count) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::GetFrameCount, count=%p\n", this, count);

        // Only 1 frame is supported by this implementation (no real world samples are known that have more)
        *check_out_pointer(count) = 1;
        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetFrame(const uint32_t index, _Outptr_ IWICBitmapFrameDecode** bitmap_frame_decode) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::GetFrame, index=%d, bitmap_frame_decode=%p\n", this, index, bitmap_frame_decode);

        check_condition(index == 0, wincodec::error_frame_missing);

        WARNING_SUPPRESS_NEXT_LINE(26447) // noexcept: false warning, caused by scoped_lock
        scoped_lock lock{mutex_};

        check_condition(static_cast<bool>(stream_reader_), wincodec::error_not_initialized);

        if (!bitmap_frame_decode_)
        {
            bitmap_frame_decode_ = winrt::make<netpbm_bitmap_frame_decode>(*stream_reader_, imaging_factory());
        }

        bitmap_frame_decode_.copy_to(check_out_pointer(bitmap_frame_decode));
        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
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

    std::mutex mutex_;
    com_ptr<IWICImagingFactory> imaging_factory_;
    com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode_;
    std::unique_ptr<buffered_stream_reader> stream_reader_;
};

export HRESULT create_netpbm_bitmap_decoder_factory(_In_ GUID const& interface_id, _Outptr_ void** result)
{
    return winrt::make<class_factory<netpbm_bitmap_decoder>>()->QueryInterface(interface_id, result);
}
