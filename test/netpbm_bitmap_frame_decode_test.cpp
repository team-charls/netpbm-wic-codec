// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "factory.h"
#include "util.h"

import test.errors;
import portable_anymap_file;

import <array>;
import <span>;
import <vector>;
import <utility>;

using std::array;
using std::span;
using std::vector;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::hresult;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace {

[[nodiscard]] std::pair<uint32_t, uint32_t> get_size(IWICBitmapFrameDecode& bitmap_frame_decoder)
{
    uint32_t width;
    uint32_t height;
    check_hresult(bitmap_frame_decoder.GetSize(&width, &height));

    return {width, height};
}

[[nodiscard]] std::vector<std::byte> unpack_crumbs(const std::byte* crumbs_pixels, const size_t width, const size_t height,
                                                   const size_t stride)
{
    std::vector<std::byte> destination(static_cast<size_t>(width) * height);

    for (size_t j{}, row{}; row != height; ++row)
    {
        const std::byte* crumbs_row{crumbs_pixels + (row * stride)};
        size_t i{};
        for (; i != width / 4; ++i)
        {
            destination[j] = crumbs_row[i] >> 6;
            ++j;
            destination[j] = (crumbs_row[i] & std::byte{0x30}) >> 4;
            ++j;
            destination[j] = (crumbs_row[i] & std::byte{0x0C}) >> 2;
            ++j;
            destination[j] = crumbs_row[i] & std::byte{0x03};
            ++j;
        }
        switch (width % 4)
        {
        case 3:
            destination[j] = crumbs_row[i] >> 6;
            ++j;
            [[fallthrough]];
        case 2:
            destination[j] = (crumbs_row[i] & std::byte{0x30}) >> 4;
            ++j;
            [[fallthrough]];
        case 1:
            destination[j] = (crumbs_row[i] & std::byte{0x0C}) >> 2;
            ++j;
            break;

        default:
            break;
        }
    }

    return destination;
}

[[nodiscard]] std::vector<std::byte> unpack_nibbles(const std::byte* nibble_pixels, const size_t width, const size_t height,
                                                    const size_t stride)
{
    std::vector<std::byte> destination(static_cast<size_t>(width) * height);

    for (size_t j{}, row{}; row != height; ++row)
    {
        const std::byte* nibble_row{nibble_pixels + (row * stride)};
        for (size_t i{}; i != width / 2; ++i)
        {
            destination[j] = nibble_row[i] >> 4;
            ++j;
            destination[j] = nibble_row[i] & std::byte{0x0F};
            ++j;
        }
    }

    return destination;
}

} // namespace


TEST_CLASS(netpbm_bitmap_frame_decode_test)
{
public:
    TEST_METHOD(GetSize) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        uint32_t width;
        uint32_t height;

        const hresult result = bitmap_frame_decoder->GetSize(&width, &height);
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(512U, width);
        Assert::AreEqual(512U, height);
    }

    TEST_METHOD(CopyPalette) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        com_ptr<IWICPalette> palette;
        check_hresult(imaging_factory()->CreatePalette(palette.put()));

        const hresult result = bitmap_frame_decoder->CopyPalette(palette.get());
        Assert::AreEqual(wincodec::error_palette_unavailable, result);
    }

    TEST_METHOD(GetThumbnail) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        com_ptr<IWICBitmapSource> thumbnail;
        const hresult result = bitmap_frame_decoder->GetThumbnail(thumbnail.put());
        Assert::AreEqual(wincodec::error_codec_no_thumbnail, result);
    }

    TEST_METHOD(GetPixelFormat_8bit_image) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        GUID pixel_format;
        const hresult result{bitmap_frame_decoder->GetPixelFormat(&pixel_format)};
        Assert::AreEqual(error_ok, result);
        Assert::IsTrue(GUID_WICPixelFormat8bppGray == pixel_format);
    }

    TEST_METHOD(GetPixelFormat_12bit_image) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"medical-m612-12bit.pgm")};

        GUID pixel_format;
        const hresult result{bitmap_frame_decoder->GetPixelFormat(&pixel_format)};
        Assert::AreEqual(error_ok, result);
        Assert::IsTrue(GUID_WICPixelFormat16bppGray == pixel_format);
    }

    TEST_METHOD(GetPixelFormat_with_nullptr) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const hresult result{bitmap_frame_decoder->GetPixelFormat(nullptr)};
        Assert::AreEqual(error_invalid_argument, result);
    }

    TEST_METHOD(GetResolution) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        double dpi_x;
        double dpi_y;
        const hresult result{bitmap_frame_decoder->GetResolution(&dpi_x, &dpi_y)};
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(96., dpi_x);
        Assert::AreEqual(96., dpi_y);
    }

    TEST_METHOD(GetResolution_with_nullptr) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const hresult result{bitmap_frame_decoder->GetResolution(nullptr, nullptr)};
        Assert::AreEqual(error_invalid_argument, result);
    }

    TEST_METHOD(GetColorContexts) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        uint32_t actual_count;
        hresult result{bitmap_frame_decoder->GetColorContexts(0, nullptr, &actual_count)};
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(0U, actual_count);

        array<IWICColorContext*, 1> color_contexts{};
        result = bitmap_frame_decoder->GetColorContexts(static_cast<UINT>(color_contexts.size()), color_contexts.data(),
                                                        &actual_count);
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(0U, actual_count);
    }

    TEST_METHOD(GetMetadataQueryReader) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        com_ptr<IWICMetadataQueryReader> metadata_query_reader;
        const hresult result{bitmap_frame_decoder->GetMetadataQueryReader(metadata_query_reader.put())};
        Assert::AreEqual(wincodec::error_unsupported_operation, result);
    }

    TEST_METHOD(CopyPixels) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        uint32_t width;
        uint32_t height;

        check_hresult(bitmap_frame_decoder->GetSize(&width, &height));
        std::vector<BYTE> buffer(static_cast<size_t>(width) * height);

        hresult result{
            bitmap_frame_decoder->CopyPixels(nullptr, width, static_cast<uint32_t>(buffer.size()), buffer.data())};
        Assert::AreEqual(error_ok, result);

        result = bitmap_frame_decoder->CopyPixels(nullptr, width, static_cast<uint32_t>(buffer.size()), buffer.data());
        Assert::AreEqual(error_ok, result);
    }

    TEST_METHOD(IsIWICBitmapSource) // NOLINT
    {
        const com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"tulips-gray-8bit-512-512.pgm");

        const com_ptr<IWICBitmapSource> bitmap_source(bitmap_frame_decoder);
        Assert::IsTrue(bitmap_source.get() != nullptr);
    }

    TEST_METHOD(decode_2_bit_monochrome_4_pixels) // NOLINT
    {
        decode_2_bit_monochrome(L"2bit_4x1.pgm", "2bit_4x1.pgm");
    }

    TEST_METHOD(decode_2_bit_monochrome_5_pixels) // NOLINT
    {
        decode_2_bit_monochrome(L"2bit_5x1.pgm", "2bit_5x1.pgm");
    }

    TEST_METHOD(decode_2_bit_monochrome_6_pixels) // NOLINT
    {
        decode_2_bit_monochrome(L"2bit_6x1.pgm", "2bit_6x1.pgm");
    }

    TEST_METHOD(decode_2_bit_monochrome_7_pixels) // NOLINT
    {
        decode_2_bit_monochrome(L"2bit_7x1.pgm", "2bit_7x1.pgm");
    }

    TEST_METHOD(decode_2_bit_monochrome_parrot) // NOLINT
    {
        decode_2_bit_monochrome(L"2bit_parrot_150x200.pgm", "2bit_parrot_150x200.pgm");
    }

    TEST_METHOD(decode_4bit_monochrome) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"4bit-monochrome.pgm")};

        const auto [width, height]{get_size(*bitmap_frame_decoder)};
        vector<std::byte> buffer(static_cast<size_t>(width) * height);

        const uint32_t stride{width / 2};
        const hresult result{copy_pixels(bitmap_frame_decoder.get(), stride, buffer.data(), buffer.size())};
        Assert::AreEqual(error_ok, result);

        const std::vector decoded_buffer{unpack_nibbles(buffer.data(), width, height, stride)};
        compare("4bit-monochrome.pgm", decoded_buffer);
    }

    TEST_METHOD(decode_8bit_monochrome) // NOLINT
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(L"tulips-gray-8bit-512-512.pgm")};

        uint32_t width;
        uint32_t height;

        check_hresult(bitmap_frame_decoder->GetSize(&width, &height));
        vector<std::byte> buffer(static_cast<size_t>(width) * height);

        const hresult result{copy_pixels(bitmap_frame_decoder.get(), width, buffer.data(), buffer.size())};
        Assert::AreEqual(error_ok, result);

        compare("tulips-gray-8bit-512-512.pgm", buffer);
    }

    TEST_METHOD(decode_10bit_monochrome) // NOLINT
    {
        decode_2_byte_samples_monochrome(L"medical-10bit.pgm", "medical-10bit.pgm");
    }

    TEST_METHOD(decode_12bit_monochrome) // NOLINT
    {
        decode_2_byte_samples_monochrome(L"medical-m612-12bit.pgm", "medical-m612-12bit.pgm");
    }

    TEST_METHOD(decode_16bit_monochrome) // NOLINT
    {
        decode_2_byte_samples_monochrome(L"640_480_16bit.pgm", "640_480_16bit.pgm");
    }

private:
    void decode_2_bit_monochrome(_Null_terminated_ const wchar_t* filename_actual, const char* filename_expected) const
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(filename_actual)};

        const auto [width, height]{get_size(*bitmap_frame_decoder)};
        vector<std::byte> buffer(static_cast<size_t>(width) * height);

        const uint32_t stride{(width + 15) / 16 * 4};
        const hresult result{copy_pixels(bitmap_frame_decoder.get(), stride, buffer.data(), buffer.size())};
        Assert::AreEqual(error_ok, result);

        const std::vector decoded_buffer{unpack_crumbs(buffer.data(), width, height, stride)};
        compare(filename_expected, decoded_buffer);
    }


    void decode_2_byte_samples_monochrome(const wchar_t* filename_actual, const char* filename_expected) const
    {
        const com_ptr bitmap_frame_decoder{create_frame_decoder(filename_actual)};

        uint32_t width;
        uint32_t height;

        check_hresult(bitmap_frame_decoder->GetSize(&width, &height));
        vector<uint16_t> buffer(static_cast<size_t>(width) * height);

        const hresult result{copy_pixels(bitmap_frame_decoder.get(), width * 2, buffer.data(), buffer.size() * 2)};
        Assert::AreEqual(error_ok, result);

        compare(filename_expected, buffer);
    }

    [[nodiscard]] com_ptr<IWICBitmapFrameDecode> create_frame_decoder(_Null_terminated_ const wchar_t* filename) const
    {
        com_ptr<IStream> stream;
        check_hresult(SHCreateStreamOnFileEx(filename, STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, nullptr, stream.put()));

        const com_ptr wic_bitmap_decoder{factory_.create_decoder()};
        check_hresult(wic_bitmap_decoder->Initialize(stream.get(), WICDecodeMetadataCacheOnDemand));

        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode;
        check_hresult(wic_bitmap_decoder->GetFrame(0, bitmap_frame_decode.put()));

        return bitmap_frame_decode;
    }

    [[nodiscard]] static com_ptr<IWICImagingFactory> imaging_factory()
    {
        com_ptr<IWICImagingFactory> imaging_factory;
        check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                                       imaging_factory.put_void()));

        return imaging_factory;
    }

    static hresult copy_pixels(IWICBitmapFrameDecode * decoder, const uint32_t stride, void* buffer,
                               const size_t buffer_size)
    {
        return decoder->CopyPixels(nullptr, stride, static_cast<uint32_t>(buffer_size), static_cast<BYTE*>(buffer));
    }

    constexpr static void convert_to_little_endian_and_shift(span<uint16_t> samples, const uint32_t sample_shift) noexcept
    {
        std::ranges::transform(samples, samples.begin(), [sample_shift](const uint16_t sample) noexcept -> uint16_t {
            return _byteswap_ushort(sample) << sample_shift;
        });
    }

    static void compare(const char* filename, const vector<std::byte>& pixels)
    {
        portable_anymap_file anymap_file{filename};
        const auto& expected_pixels{anymap_file.image_data()};

        for (size_t i{}; i < pixels.size(); ++i)
        {
            if (expected_pixels[i] != pixels[i])
            {
                Assert::IsTrue(false);
                break;
            }
        }
    }

    static void compare(const char* filename, const vector<uint16_t>& pixels)
    {
        portable_anymap_file anymap_file{filename};
        auto& expected_pixels{anymap_file.image_data()};

        const span expected{reinterpret_cast<uint16_t*>(expected_pixels.data()), expected_pixels.size() / sizeof uint16_t};
        convert_to_little_endian_and_shift(expected, 16 - anymap_file.bits_per_sample());

        for (size_t i{}; i < pixels.size(); ++i)
        {
            if (expected[i] != pixels[i])
            {
                Assert::IsTrue(false);
                break;
            }
        }
    }

    factory factory_;
};
