// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "macros.hpp"

module netpbm_bitmap_decoder;

import std;
import <win.h>;
import winrt;

import class_factory;
import errors;
import pnm_header;
import guids;
import netpbm_bitmap_frame_decode;
import util;

using std::uint32_t;
using std::scoped_lock;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::to_hresult;

namespace {

struct netpbm_bitmap_decoder : winrt::implements<netpbm_bitmap_decoder, IWICBitmapDecoder>
{
    // IWICBitmapDecoder
    HRESULT __stdcall QueryCapability(_In_ IStream* stream, _Out_ DWORD* capability) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_decoder::QueryCapability.1, stream address={}, capability address={}\n", fmt_ptr(this),
              fmt_ptr(stream), fmt_ptr(capability));

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

        TRACE("{} netpbm_bitmap_decoder::QueryCapability.2, *capability={}\n", fmt_ptr(this), *capability);
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
        TRACE("{} netpbm_bitmap_decoder::Initialize, stream address={}, cache_options={}\n", fmt_ptr(this), fmt_ptr(stream),
              static_cast<int>(cache_options));

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
        TRACE("{} netpbm_bitmap_decoder::GetContainerFormat, container_format address={}\n", fmt_ptr(this),
              fmt_ptr(container_format));

        *check_out_pointer(container_format) = id::container_format_netpbm;
        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetDecoderInfo(_Outptr_ IWICBitmapDecoderInfo** decoder_info) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_decoder::GetContainerFormat, decoder_info address={}\n", fmt_ptr(this),
              fmt_ptr(decoder_info));

        com_ptr<IWICComponentInfo> component_info;
        check_hresult(imaging_factory()->CreateComponentInfo(id::netpbm_decoder, component_info.put()));
        check_hresult(component_info->QueryInterface(IID_PPV_ARGS(decoder_info)));

        return error_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall CopyPalette([[maybe_unused]] _In_ IWICPalette* palette) noexcept override
    {
        TRACE("{} netpbm_bitmap_decoder::CopyPalette, palette address={}\n", fmt_ptr(this), fmt_ptr(palette));

        // NetPbm images don't have palettes.
        return wincodec::error_palette_unavailable;
    }

    HRESULT __stdcall GetMetadataQueryReader(
        [[maybe_unused]] _Outptr_ IWICMetadataQueryReader** metadata_query_reader) noexcept override
    {
        TRACE("{} netpbm_bitmap_decoder::GetMetadataQueryReader (not supported), metadata_query_reader address={}\n",
              fmt_ptr(this), fmt_ptr(metadata_query_reader));

        // Keep the initial design simple: no support for container-level metadata.
        // Note: Conceptual, comments from the NetPbm file could converted into metadata.
        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall GetPreview([[maybe_unused]] _Outptr_ IWICBitmapSource** bitmap_source) noexcept override
    {
        TRACE("{} netpbm_bitmap_decoder::GetPreview (not supported), bitmap_source address={}\n", fmt_ptr(this),
              fmt_ptr(bitmap_source));

        // The Netpbm format doesn't support storing previews in the file format.
        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall GetColorContexts([[maybe_unused]] const uint32_t count,
                                       [[maybe_unused]] IWICColorContext** color_contexts,
                                       [[maybe_unused]] uint32_t* actual_count) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_decoder::GetColorContexts (always 0), count={}, color_contexts address={}, actual_count "
              "address={}\n",
              fmt_ptr(this), count, fmt_ptr(color_contexts), fmt_ptr(actual_count));

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
        TRACE("%p netpbm_bitmap_decoder::GetThumbnail (not supported), thumbnail address=%p\n", fmt_ptr(this),
              fmt_ptr(thumbnail));

        // The Netpbm format doesn't support storing thumbnails in the file format.
        return wincodec::error_codec_no_thumbnail;
    }

    HRESULT __stdcall GetFrameCount(_Out_ uint32_t* count) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_decoder::GetFrameCount (always 1), count address={}\n", fmt_ptr(this), fmt_ptr(count));

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
        TRACE("{} netpbm_bitmap_decoder::GetFrame, index={}, bitmap_frame_decode address={}\n", fmt_ptr(this), index,
              fmt_ptr(bitmap_frame_decode));

        check_condition(index == 0, wincodec::error_frame_missing);

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

} // namespace

void create_netpbm_bitmap_decoder_factory(GUID const& interface_id, void** result)
{
    check_hresult(winrt::make<class_factory<netpbm_bitmap_decoder>>()->QueryInterface(interface_id, result));
}
