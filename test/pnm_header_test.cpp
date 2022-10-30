// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "cpp_unit_test.h"
#include "winrt.h"

import pnm_header;
import test_util;

using winrt::com_ptr;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(pnm_header_test)
{
public:
    TEST_METHOD(is_pnm_file_for_p5) // NOLINT
    {
        constexpr std::byte initial_values[]{std::byte{'P'}, std::byte{'5'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsTrue(result);
    }

    TEST_METHOD(is_pnm_file_for_p6) // NOLINT
    {
        constexpr std::byte initial_values[]{std::byte{'P'}, std::byte{'6'}};
        const com_ptr stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }
};
