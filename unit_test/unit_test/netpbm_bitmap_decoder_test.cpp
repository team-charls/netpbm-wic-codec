// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "factory.h"
#include "util.h"

#include <CppUnitTest.h>
#include <shlwapi.h>

import errors;

using namespace winrt;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// {70ab66f5-cd48-43a1-aa29-10131b7f4ff1}
constexpr GUID GUID_ContainerFormatNetPbm{0x70ab66f5, 0xcd48, 0x43a1, {0xaa, 0x29, 0x10, 0x13, 0x1b, 0x7f, 0x4f, 0xf1}};


TEST_CLASS(netpbm_bitmap_decoder_test)
{
public:
    TEST_METHOD(GetContainerFormat) // NOLINT
    {
        GUID container_format;
        const hresult result = factory_.create_decoder()->GetContainerFormat(&container_format);

        Assert::AreEqual(error_ok, result);
        Assert::IsTrue(GUID_ContainerFormatNetPbm == container_format);
    }

    TEST_METHOD(GetContainerFormat_with_nullptr) // NOLINT
    {
        WARNING_SUPPRESS_NEXT_LINE(6387) // don't pass nullptr
        const hresult result = factory_.create_decoder()->GetContainerFormat(nullptr);

        Assert::IsTrue(failed(result));
    }

    TEST_METHOD(GetDecoderInfo) // NOLINT
    {
        com_ptr<IWICBitmapDecoder> decoder = factory_.create_decoder();

        com_ptr<IWICBitmapDecoderInfo> decoder_info;
        const hresult result = decoder->GetDecoderInfo(decoder_info.put());

        Assert::IsTrue(result == error_ok || result == wincodec::error_component_not_found);
        if (succeeded(result))
        {
            Assert::IsNotNull(decoder_info.get());
        }
        else
        {
            Assert::IsNull(decoder_info.get());
        }
    }

    TEST_METHOD(GetDecoderInfo_with_nullptr) // NOLINT
    {
        WARNING_SUPPRESS_NEXT_LINE(6387) // don't pass nullptr
        const hresult result{factory_.create_decoder()->GetDecoderInfo(nullptr)};

        Assert::IsTrue(failed(result));
    }

    TEST_METHOD(CopyPalette) // NOLINT
    {
        const com_ptr<IWICPalette> palette;
        const hresult result = factory_.create_decoder()->CopyPalette(palette.get());

        Assert::AreEqual(wincodec::error_palette_unavailable, result);
    }

    TEST_METHOD(GetMetadataQueryReader) // NOLINT
    {
        com_ptr<IWICMetadataQueryReader> metadata_query_reader;
        const hresult result = factory_.create_decoder()->GetMetadataQueryReader(metadata_query_reader.put());

        Assert::AreEqual(wincodec::error_unsupported_operation, result);
    }

    TEST_METHOD(GetPreview) // NOLINT
    {
        com_ptr<IWICBitmapSource> bitmap_source;
        const hresult result = factory_.create_decoder()->GetPreview(bitmap_source.put());

        Assert::AreEqual(wincodec::error_unsupported_operation, result);
    }

    TEST_METHOD(GetColorContexts) // NOLINT
    {
        com_ptr<IWICColorContext> color_contexts;
        uint32_t actual_count;
        const hresult result = factory_.create_decoder()->GetColorContexts(1, color_contexts.put(), &actual_count);

        Assert::AreEqual(wincodec::error_unsupported_operation, result);
    }

    TEST_METHOD(GetThumbnail) // NOLINT
    {
        com_ptr<IWICBitmapSource> bitmap_source;
        const hresult result = factory_.create_decoder()->GetThumbnail(bitmap_source.put());

        Assert::AreEqual(wincodec::error_codec_no_thumbnail, result);
    }

    TEST_METHOD(GetFrameCount) // NOLINT
    {
        uint32_t frame_count;
        const hresult result = factory_.create_decoder()->GetFrameCount(&frame_count);

        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(1U, frame_count);
    }

    TEST_METHOD(GetFrameCount_count_parameter_is_null) // NOLINT
    {
        WARNING_SUPPRESS_NEXT_LINE(6387) // don't pass nullptr
        const hresult result{factory_.create_decoder()->GetFrameCount(nullptr)};

        Assert::AreEqual(error_pointer, result);
    }

    TEST_METHOD(QueryCapability_cannot_decode_empty) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        DWORD capability;
        const hresult result{factory_.create_decoder()->QueryCapability(stream.get(), &capability)};

        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(0UL, capability);
    }

    TEST_METHOD(QueryCapability_stream_argument_null) // NOLINT
    {
        DWORD capability;
        const hresult result{factory_.create_decoder()->QueryCapability(nullptr, &capability)};

        Assert::AreEqual(error_invalid_argument, result);
    }

    TEST_METHOD(QueryCapability_capability_argument_null) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        WARNING_SUPPRESS_NEXT_LINE(6387) // don't pass nullptr
        const hresult result{factory_.create_decoder()->QueryCapability(stream.get(), nullptr)};

        Assert::AreEqual(error_pointer, result);
    }

    TEST_METHOD(QueryCapability_can_decode_8bit_monochrome) // NOLINT
    {
        com_ptr<IStream> stream;
        check_hresult(SHCreateStreamOnFileEx(L"lena8b.pgm", STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, nullptr, stream.put()));
        DWORD capability;
        const hresult result = factory_.create_decoder()->QueryCapability(stream.get(), &capability);

        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(static_cast<DWORD>(WICBitmapDecoderCapabilityCanDecodeAllImages), capability);
    }

private:
    factory factory_;
};
