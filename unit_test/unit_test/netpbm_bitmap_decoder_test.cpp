// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "factory.h"
#include "util.h"

#include <CppUnitTest.h>

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

private:
    factory factory_;
};
