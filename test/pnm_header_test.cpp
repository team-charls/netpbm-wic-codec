// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

//#include "pch.h"

#include "cpp_unit_test.h"
#include "winrt.h"

import pnm_header;
import test_util;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using std::byte;

TEST_CLASS(pnm_header_test)
{
public:
    TEST_METHOD(is_pnm_file_for_p5) // NOLINT
    {
        byte initial_values[]{static_cast<byte>('P'), static_cast<byte>('5')};
        winrt::com_ptr<IStream> stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsTrue(result);
    }

    TEST_METHOD(is_pnm_file_for_p6) // NOLINT
    {
        byte initial_values[]{static_cast<byte>('P'), static_cast<byte>('6')};
        winrt::com_ptr<IStream> stream{create_memory_stream(initial_values)};

        const bool result{is_pnm_file(stream.get())};
        Assert::IsFalse(result);
    }
};
