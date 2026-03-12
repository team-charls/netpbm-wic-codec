// SPDX-FileCopyrightText: © 2021 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

#include "cpp_unit_test.hpp"

import std;
import <win.hpp>;
import test.winrt;

import pnm_header;
import buffered_stream_reader;
import test.util;

using std::array;
using std::byte;
using winrt::com_ptr;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(pnm_header_test)
{
public:
    TEST_METHOD(is_pnm_file_for_p1) // NOLINT
    {
        constexpr array initial_values{byte{'P'}, byte{'1'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p2) // NOLINT
    {
        constexpr array initial_values{byte{'P'}, byte{'2'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p3) // NOLINT
    {
        constexpr array initial_values{byte{'P'}, byte{'3'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p4) // NOLINT
    {
        constexpr array initial_values{byte{'P'}, byte{'4'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p5) // NOLINT
    {
        constexpr array initial_values{byte{'P'}, byte{'5'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsTrue(result);
    }

    TEST_METHOD(is_pnm_file_for_p6) // NOLINT
    {
        constexpr array initial_values{byte{'P'}, byte{'6'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsTrue(result);
    }

    TEST_METHOD(is_pnm_file_for_p7) // NOLINT
    {
        constexpr array initial_values{byte{'P'}, byte{'7'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsTrue(result);
    }

    TEST_METHOD(parse_pam) // NOLINT
    {
        std::vector<char> source;
        std::string str1 = "P7\n";
        source.insert(source.end(), str1.begin(), str1.end());
        str1 = "HEIGHT 100\n";
        source.insert(source.end(), str1.begin(), str1.end());
        str1 = "WIDTH 200\n";
        source.insert(source.end(), str1.begin(), str1.end());
        str1 = "DEPTH 4\n";
        source.insert(source.end(), str1.begin(), str1.end());
        str1 = "MAXVAL 255\n";
        source.insert(source.end(), str1.begin(), str1.end());
        str1 = "ENDHDR\n";
        source.insert(source.end(), str1.begin(), str1.end());

        const com_ptr stream{create_memory_stream(source.data(), source.size())};

        buffered_stream_reader reader{stream.get()};
        pnm_header header{reader};

        Assert::AreEqual(100U, header.height);
        Assert::AreEqual(200U, header.width);
        Assert::AreEqual(static_cast<USHORT>(255U), header.MaxColorValue);
        Assert::IsTrue(PnmType::ArbitraryMap == header.PnmType);
    }
};
