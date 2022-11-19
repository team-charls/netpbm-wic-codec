// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "macros.h"
#include "winrt.h"

module netpbm_bitmap_decoder;

import class_factory;
import errors;
import pnm_header;
import guids;
import netpbm_bitmap_frame_decode;
import trace;
import <mutex>;
import <wincodec.h>;

using std::scoped_lock;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::to_hresult;


class netpbm_bitmap_decoder final : public winrt::implements<netpbm_bitmap_decoder, IWICBitmapDecoder>
{
public:
    // IWICBitmapDecoder
    HRESULT __stdcall QueryCapability(_In_ IStream* stream, _Out_ DWORD* capability) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::QueryCapability.1, stream address=%p, capability address=%p\n", this, stream,
              capability);

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

        TRACE("%p netpbm_bitmap_decoder::QueryCapability.2, *capability=%d\n", this, *capability);
        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall Initialize(_In_ IStream* stream,
                                 [[maybe_unused]] const WICDecodeOptions cache_options) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::Initialize, stream address=%p, cache_options=%d\n", this, stream, cache_options);

        SUPPRESS_FALSE_WARNING_C26447_NEXT_LINE
        scoped_lock lock{mutex_};
        source_stream_.copy_from(check_in_pointer(stream));
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
        TRACE("%p netpbm_bitmap_decoder::GetContainerFormat, container_format address=%p\n", this, container_format);

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
        TRACE("%p netpbm_bitmap_decoder::GetContainerFormat, decoder_info address=%p\n", this, decoder_info);

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
        TRACE("%p netpbm_bitmap_decoder::CopyPalette, palette address=%p\n", this, palette);

        // NetPbm images don't have palettes.
        return wincodec::error_palette_unavailable;
    }

    HRESULT __stdcall GetMetadataQueryReader(
        [[maybe_unused]] _Outptr_ IWICMetadataQueryReader** metadata_query_reader) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetMetadataQueryReader (not supported), metadata_query_reader address=%p\n", this,
              metadata_query_reader);

        // Keep the initial design simple: no support for container-level metadata.
        // Note: Conceptual, comments from the NetPbm file could converted into metadata.
        constexpr HRESULT hr = wincodec::error_unsupported_operation;
        return hr;
    }

    HRESULT __stdcall GetPreview([[maybe_unused]] _Outptr_ IWICBitmapSource** bitmap_source) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetPreview (not supported), bitmap_source address=%p\n", this, bitmap_source);

        // The Netpbm format doesn't support storing previews in the file format.
        constexpr HRESULT hr = wincodec::error_unsupported_operation;
        return hr;
    }

    HRESULT __stdcall GetColorContexts([[maybe_unused]] const uint32_t count,
                                       [[maybe_unused]] IWICColorContext** color_contexts,
                                       [[maybe_unused]] uint32_t* actual_count) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::GetColorContexts (always 0), count=%u, color_contexts address=%p, actual_count "
              "address=%p\n",
              this, count, color_contexts, actual_count);

        // The Netpbm format doesn't support storing color contexts (ICC profiles) in the file format.
        *check_out_pointer(actual_count) = 0;
        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetThumbnail([[maybe_unused]] _Outptr_ IWICBitmapSource** thumbnail) noexcept override
    {
        TRACE("%p netpbm_bitmap_decoder::GetThumbnail (not supported), thumbnail address=%p\n", this, thumbnail);

        // The Netpbm format doesn't support storing thumbnails in the file format.
        constexpr HRESULT hr{wincodec::error_codec_no_thumbnail};
        return hr;
    }

    HRESULT __stdcall GetFrameCount(_Out_ uint32_t* count) noexcept override
    try
    {
        TRACE("%p netpbm_bitmap_decoder::GetFrameCount (always 1), count address=%p\n", this, count);

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
        TRACE("%p netpbm_bitmap_decoder::GetFrame, index=%d, bitmap_frame_decode address=%p\n", this, index,
              bitmap_frame_decode);

        check_condition(index == 0, wincodec::error_frame_missing);

        SUPPRESS_FALSE_WARNING_C26447_NEXT_LINE
        scoped_lock lock{mutex_};
        check_condition(static_cast<bool>(source_stream_), wincodec::error_not_initialized);

        if (!bitmap_frame_decode_)
        {
            bitmap_frame_decode_ = winrt::make<netpbm_bitmap_frame_decode>(source_stream_.get(), imaging_factory());
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
            check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                           IID_PPV_ARGS(imaging_factory_.put())));
        }

        return imaging_factory_.get();
    }

    std::mutex mutex_;
    com_ptr<IWICImagingFactory> imaging_factory_;
    com_ptr<IStream> source_stream_;
    com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode_;
};


HRESULT create_netpbm_bitmap_decoder_factory(GUID const& interface_id, void** result)
{
    const HRESULT hr{winrt::make<class_factory<netpbm_bitmap_decoder>>()->QueryInterface(interface_id, result)};
    return hr;
}
