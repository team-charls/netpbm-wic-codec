// SPDX-FileCopyrightText: © 2020 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

#include "macros.hpp"
#include "intellisense.hpp"
#include "cpp_unit_test.hpp"

import std;
import <win.hpp>;

import test.winrt;
import test.hresults;
import test.stream;
import test.util;
import com_factory;

using namespace winrt;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using std::vector;

// {70ab66f5-cd48-43a1-aa29-10131b7f4ff1}
constexpr GUID container_format_netpbm{0x70ab66f5, 0xcd48, 0x43a1, {0xaa, 0x29, 0x10, 0x13, 0x1b, 0x7f, 0x4f, 0xf1}};


TEST_CLASS(netpbm_bitmap_decoder_test)
{
public:
    TEST_METHOD(GetContainerFormat) // NOLINT
    {
        GUID container_format;
        const auto result{codec_factory_.create_decoder()->GetContainerFormat(&container_format)};

        Assert::AreEqual(success_ok, result);
        Assert::IsTrue(container_format_netpbm == container_format);
    }

    TEST_METHOD(GetContainerFormat_with_nullptr) // NOLINT
    {
        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const auto result{codec_factory_.create_decoder()->GetContainerFormat(nullptr)};

        Assert::IsTrue(failed(result));
    }

    TEST_METHOD(GetDecoderInfo) // NOLINT
    {
        const com_ptr decoder{codec_factory_.create_decoder()};

        com_ptr<IWICBitmapDecoderInfo> decoder_info;
        const auto result{decoder->GetDecoderInfo(decoder_info.put())};

        Assert::IsTrue(result == success_ok || result == wincodec::error_component_not_found);
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
        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const auto result{codec_factory_.create_decoder()->GetDecoderInfo(nullptr)};

        Assert::IsTrue(failed(result));
    }

    TEST_METHOD(CopyPalette) // NOLINT
    {
        const com_ptr<IWICPalette> palette;
        const auto result{codec_factory_.create_decoder()->CopyPalette(palette.get())};

        Assert::AreEqual(wincodec::error_palette_unavailable, result);
    }

    TEST_METHOD(GetMetadataQueryReader) // NOLINT
    {
        com_ptr<IWICMetadataQueryReader> metadata_query_reader;
        const auto result{codec_factory_.create_decoder()->GetMetadataQueryReader(metadata_query_reader.put())};

        Assert::AreEqual(wincodec::error_unsupported_operation, result);
    }

    TEST_METHOD(GetPreview) // NOLINT
    {
        com_ptr<IWICBitmapSource> bitmap_source;
        const auto result{codec_factory_.create_decoder()->GetPreview(bitmap_source.put())};

        Assert::AreEqual(wincodec::error_unsupported_operation, result);
    }

    TEST_METHOD(GetColorContexts) // NOLINT
    {
        com_ptr<IWICColorContext> color_contexts;
        uint32_t actual_count;
        const auto result{codec_factory_.create_decoder()->GetColorContexts(1, color_contexts.put(), &actual_count)};

        Assert::AreEqual(success_ok, result);
        Assert::AreEqual(0U, actual_count);
    }

    TEST_METHOD(GetThumbnail) // NOLINT
    {
        com_ptr<IWICBitmapSource> bitmap_source;
        const auto result{codec_factory_.create_decoder()->GetThumbnail(bitmap_source.put())};

        Assert::AreEqual(wincodec::error_codec_no_thumbnail, result);
    }

    TEST_METHOD(GetFrameCount) // NOLINT
    {
        uint32_t frame_count;
        const auto result{codec_factory_.create_decoder()->GetFrameCount(&frame_count)};

        Assert::AreEqual(success_ok, result);
        Assert::AreEqual(1U, frame_count);
    }

    TEST_METHOD(GetFrameCount_count_parameter_is_null) // NOLINT
    {
        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const auto result{codec_factory_.create_decoder()->GetFrameCount(nullptr)};

        Assert::AreEqual(error_pointer, result);
    }

    TEST_METHOD(QueryCapability_cannot_decode_empty) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        DWORD capability;
        const auto result{codec_factory_.create_decoder()->QueryCapability(stream.get(), &capability)};

        Assert::AreEqual(success_ok, result);
        Assert::AreEqual(0UL, capability);
    }

    TEST_METHOD(QueryCapability_stream_argument_null) // NOLINT
    {
        DWORD capability;
        const auto result{codec_factory_.create_decoder()->QueryCapability(nullptr, &capability)};

        Assert::AreEqual(error_invalid_argument, result);
    }

    TEST_METHOD(QueryCapability_capability_argument_null) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const auto result{codec_factory_.create_decoder()->QueryCapability(stream.get(), nullptr)};

        Assert::AreEqual(error_pointer, result);
    }

    TEST_METHOD(QueryCapability_can_decode_8bit_monochrome) // NOLINT
    {
        com_ptr<IStream> stream;
        check_hresult(
            SHCreateStreamOnFileEx(L"tulips-gray-8bit-512-512.pgm", STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, nullptr, stream.put()));
        DWORD capability;
        const auto result{codec_factory_.create_decoder()->QueryCapability(stream.get(), &capability)};

        Assert::AreEqual(success_ok, result);
        Assert::AreEqual(static_cast<DWORD>(WICBitmapDecoderCapabilityCanDecodeAllImages), capability);
    }

    TEST_METHOD(QueryCapability_read_error_on_stream) // NOLINT
    {
        const com_ptr stream{winrt::make<test_stream>(true, 2)};

        DWORD capability;
        const auto result = codec_factory_.create_decoder()->QueryCapability(stream.get(), &capability);

        Assert::IsTrue(failed(result));
    }

    TEST_METHOD(QueryCapability_seek_error_on_stream) // NOLINT
    {
        const com_ptr stream{winrt::make<test_stream>(false, 1)};

        DWORD capability;
        const auto result = codec_factory_.create_decoder()->QueryCapability(stream.get(), &capability);

        Assert::IsTrue(failed(result));
    }

    TEST_METHOD(QueryCapability_seek_error_on_stream_reset) // NOLINT
    {
        const com_ptr stream{winrt::make<test_stream>(false, 2)};

        DWORD capability;
        const auto result = codec_factory_.create_decoder()->QueryCapability(stream.get(), &capability);

        Assert::IsTrue(failed(result));
    }

    TEST_METHOD(Initialize_cache_on_demand) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        const auto result{codec_factory_.create_decoder()->Initialize(stream.get(), WICDecodeMetadataCacheOnDemand)};
        Assert::AreEqual(success_ok, result);
    }

    TEST_METHOD(Initialize_cache_on_load) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        const auto result{codec_factory_.create_decoder()->Initialize(stream.get(), WICDecodeMetadataCacheOnLoad)};
        Assert::AreEqual(success_ok, result);
    }

    TEST_METHOD(Initialize_twice) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        const com_ptr decoder{codec_factory_.create_decoder()};
        auto result{decoder->Initialize(stream.get(), WICDecodeMetadataCacheOnDemand)};
        Assert::AreEqual(success_ok, result);

        result = decoder->Initialize(stream.get(), WICDecodeMetadataCacheOnLoad);
        Assert::AreEqual(success_ok, result);
    }

    TEST_METHOD(Initialize_bad_cache_option) // NOLINT
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(nullptr, 0));

        const auto result{codec_factory_.create_decoder()->Initialize(stream.get(), static_cast<WICDecodeOptions>(4))};

        // Cache options is not used by decoder and by design not validated.
        Assert::AreEqual(success_ok, result);
    }

    TEST_METHOD(Initialize_null_stream) // NOLINT
    {
        const auto result{codec_factory_.create_decoder()->Initialize(nullptr, WICDecodeMetadataCacheOnDemand)};
        Assert::AreEqual(error_invalid_argument, result);
    }

    TEST_METHOD(GetFrame) // NOLINT
    {
        com_ptr<IStream> stream;
        check_hresult(
            SHCreateStreamOnFileEx(L"tulips-gray-8bit-512-512.pgm", STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, nullptr, stream.put()));

        com_ptr<IWICBitmapDecoder> decoder = codec_factory_.create_decoder();
        auto result{decoder->Initialize(stream.get(), WICDecodeMetadataCacheOnDemand)};
        Assert::AreEqual(success_ok, result);

        uint32_t frame_count;
        result = decoder->GetFrameCount(&frame_count);
        Assert::AreEqual(success_ok, result);
        Assert::AreEqual(1U, frame_count);

        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode;
        result = decoder->GetFrame(0, bitmap_frame_decode.put());
        Assert::AreEqual(success_ok, result);
        Assert::IsTrue(bitmap_frame_decode.get() != nullptr);
    }

    TEST_METHOD(GetFrame_with_frame_argument_null) // NOLINT
    {
        com_ptr<IStream> stream;
        check_hresult(
            SHCreateStreamOnFileEx(L"tulips-gray-8bit-512-512.pgm", STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, nullptr, stream.put()));

        com_ptr<IWICBitmapDecoder> decoder = codec_factory_.create_decoder();
        auto result{decoder->Initialize(stream.get(), WICDecodeMetadataCacheOnDemand)};
        Assert::AreEqual(success_ok, result);

        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        result = decoder->GetFrame(0, nullptr);

        Assert::AreEqual(error_pointer, result);
    }

    TEST_METHOD(GetFrame_with_bad_index) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode;
        const auto result{codec_factory_.create_decoder()->GetFrame(1, bitmap_frame_decode.put())};

        Assert::AreEqual(wincodec::error_frame_missing, result);
    }

    TEST_METHOD(GetFrame_not_initialized) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode;
        const auto result{codec_factory_.create_decoder()->GetFrame(0, bitmap_frame_decode.put())};

        Assert::AreEqual(wincodec::error_not_initialized, result);
    }

private:
    com_factory codec_factory_;
};
