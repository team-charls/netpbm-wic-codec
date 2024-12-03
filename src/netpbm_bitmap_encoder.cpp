// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "macros.hpp"

module netpbm_bitmap_encoder;

import std;
import <win.hpp>;
import winrt;

import class_factory;
import guids;
import hresults;
import util;

using std::vector;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::implements;
using winrt::make;

namespace {

struct netpbm_bitmap_encoder : implements<netpbm_bitmap_encoder, IWICBitmapEncoder>
{
    // IWICBitmapEncoder
    HRESULT __stdcall Initialize(_In_ IStream* destination,
                                 [[maybe_unused]] const WICBitmapEncoderCacheOption cache_option) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_encoder::Initialize, stream={}, cache_option={}\n", fmt_ptr(this), fmt_ptr(destination),
              std::to_underlying(cache_option));

        check_condition(!static_cast<bool>(destination_), wincodec::error_wrong_state);
        destination_.copy_from(check_in_pointer(destination));
        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetContainerFormat(_Out_ GUID* container_format) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_encoder::GetContainerFormat, container_format={}\n", fmt_ptr(this),
              fmt_ptr(container_format));

        *check_out_pointer(container_format) = id::container_format_netpbm;
        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetEncoderInfo(_Outptr_ IWICBitmapEncoderInfo** encoder_info) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_encoder::GetEncoderInfo, encoder_info={}\n", fmt_ptr(this), fmt_ptr(encoder_info));

        com_ptr<IWICComponentInfo> component_info;
        check_hresult(imaging_factory()->CreateComponentInfo(id::netpbm_encoder, component_info.put()));
        check_hresult(component_info->QueryInterface(IID_PPV_ARGS(encoder_info)));

        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall CreateNewFrame(_Outptr_ IWICBitmapFrameEncode** bitmap_frame_encode,
                                     IPropertyBag2** encoder_options) noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_encoder::CreateNewFrame, bitmap_frame_encode={}, encoder_options={}\n", fmt_ptr(this),
              fmt_ptr(bitmap_frame_encode), fmt_ptr(encoder_options));

        check_condition(static_cast<bool>(destination_), wincodec::error_not_initialized);
        ////check_condition(!static_cast<bool>(bitmap_frame_encode_), wincodec::error_wrong_state); // Only 1 frame is supported.

        ////bitmap_frame_encode_ = winrt::make_self<jpegls_bitmap_frame_encode>();

        *check_out_pointer(bitmap_frame_encode) = nullptr;         // bitmap_frame_encode_.get();
        (*bitmap_frame_encode)->AddRef();

        if (encoder_options)
        {
            *encoder_options = nullptr;
        }

        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall Commit() noexcept override
    try
    {
        TRACE("{} netpbm_bitmap_encoder::Commit\n", fmt_ptr(this));

        check_condition(!committed_, wincodec::error_wrong_state);
        check_condition(static_cast<bool>(destination_), wincodec::error_not_initialized);
        //check_condition(static_cast<bool>(bitmap_frame_encode_), wincodec::error_frame_missing);

        //jpegls_encoder encoder;
        //encoder.frame_info(bitmap_frame_encode_->frame_info());

        //vector<std::byte> destination(encoder.estimated_destination_size());
        //encoder.destination(destination);

        //if (bitmap_frame_encode_->frame_info().component_count > 1)
        //{
        //    encoder.interleave_mode(interleave_mode::sample);
        //}

        //write_spiff_header(encoder, *bitmap_frame_encode_.get());

        //const auto bytes_written{encoder.encode(bitmap_frame_encode_->source(), bitmap_frame_encode_->source_stride())};
        //bitmap_frame_encode_ = nullptr;

        //check_hresult(destination_->Write(destination.data(), static_cast<ULONG>(bytes_written), nullptr));
        check_hresult(destination_->Commit(STGC_DEFAULT));
        destination_ = nullptr;

        committed_ = true;
        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    // Optional methods
    HRESULT __stdcall SetPreview([[maybe_unused]] _In_ IWICBitmapSource* preview) noexcept override
    {
        TRACE("{} netpbm_bitmap_encoder::SetPreview, preview={}\n", fmt_ptr(this), fmt_ptr(preview));
        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall SetThumbnail([[maybe_unused]] _In_ IWICBitmapSource* thumbnail) noexcept override
    {
        TRACE("{} netpbm_bitmap_encoder::SetThumbnail, thumbnail={}\n", fmt_ptr(this), fmt_ptr(thumbnail));
        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall SetColorContexts([[maybe_unused]] const std::uint32_t count,
                                       [[maybe_unused]] IWICColorContext** color_context) noexcept override
    {
        TRACE("{} netpbm_bitmap_encoder::SetColorContexts, count={}, color_context={}\n", fmt_ptr(this), count,
              fmt_ptr(color_context));

        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall GetMetadataQueryWriter(
        [[maybe_unused]] _Outptr_ IWICMetadataQueryWriter** metadata_query_writer) noexcept override
    {
        TRACE("{} netpbm_bitmap_encoder::GetMetadataQueryWriter, metadata_query_writer={}\n", fmt_ptr(this),
              fmt_ptr(metadata_query_writer));

        return wincodec::error_unsupported_operation;
    }

    HRESULT __stdcall SetPalette(_In_ IWICPalette* palette) noexcept override
    {
        TRACE("{} jpegls_bitmap_encoder::SetPalette, palette={}\n", fmt_ptr(this), fmt_ptr(palette));

        return wincodec::error_unsupported_operation;
    }

private:
    [[nodiscard]]
    IWICImagingFactory* imaging_factory()
    {
        if (!imaging_factory_)
        {
            check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                           IID_PPV_ARGS(imaging_factory_.put())));
        }

        return imaging_factory_.get();
    }

    bool committed_{};
    com_ptr<IWICImagingFactory> imaging_factory_;
    com_ptr<IStream> destination_;
    //com_ptr<jpegls_bitmap_frame_encode> bitmap_frame_encode_;
};

} // namespace

void create_netpbm_bitmap_encoder_class_factory(GUID const& interface_id, void** result)
{
    check_hresult(make<class_factory<netpbm_bitmap_encoder>>()->QueryInterface(interface_id, result));
}
