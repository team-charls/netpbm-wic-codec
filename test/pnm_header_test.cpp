// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "cpp_unit_test.h"

import <std.h>;
import <win.h>;
import test.winrt;

import pnm_header;
import test.util;

using std::array;
using winrt::com_ptr;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(pnm_header_test)
{
public:
    TEST_METHOD(is_pnm_file_for_p1) // NOLINT
    {
        constexpr array initial_values{std::byte{'P'}, std::byte{'1'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p2) // NOLINT
    {
        constexpr array initial_values{std::byte{'P'}, std::byte{'2'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p3) // NOLINT
    {
        constexpr array initial_values{std::byte{'P'}, std::byte{'3'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p4) // NOLINT
    {
        constexpr array initial_values{std::byte{'P'}, std::byte{'4'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }

    TEST_METHOD(is_pnm_file_for_p5) // NOLINT
    {
        constexpr array initial_values{std::byte{'P'}, std::byte{'5'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsTrue(result);
    }

    TEST_METHOD(is_pnm_file_for_p6) // NOLINT
    {
        constexpr array initial_values{std::byte{'P'}, std::byte{'6'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsTrue(result);
    }

    TEST_METHOD(is_pnm_file_for_p7) // NOLINT
    {
        constexpr array initial_values{std::byte{'P'}, std::byte{'7'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }
};
