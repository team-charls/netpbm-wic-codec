// Copyright (c) Team CharLS.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "factory.h"
#include "util.h"

#include <CppUnitTest.h>

import errors;

using namespace winrt;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

 // {358bb60e-2b77-4411-8a76-27164945c93d}
constexpr GUID GUID_Random{0x358bb60e, 0x2b77, 0x4411, {0x8a, 0x76, 0x27, 0x16, 0x49, 0x45, 0xc9, 0x3d}};


TEST_CLASS(dllmain_test)
{
public:
    TEST_METHOD(class_factory_jpegls_decoder_lock_server) // NOLINT
    {
        auto class_factory = factory_.get_class_factory(CLSID_NetPbmDecoder);

        hresult result{class_factory->LockServer(true)};
        Assert::AreEqual(error_ok, result);

        result = class_factory->LockServer(false);
        Assert::AreEqual(error_ok, result);
    }

    TEST_METHOD(class_factory_unknown_id) // NOLINT
    {
        winrt::com_ptr<IClassFactory> class_factory;
        const hresult result{factory_.get_class_factory(GUID_Random, class_factory)};

        Assert::AreEqual(error_class_not_available, result);
    }

    TEST_METHOD(class_factory_jpegls_decoder_create_instance_bad_result) // NOLINT
    {
        auto class_factory = factory_.get_class_factory(CLSID_NetPbmDecoder);

        WARNING_SUPPRESS_NEXT_LINE(6387) // don't pass nullptr
        const hresult result{class_factory->CreateInstance(nullptr, GUID_Random, nullptr)};

        Assert::AreEqual(error_pointer, result);
    }

    TEST_METHOD(class_factory_jpegls_decoder_create_instance_no_aggregation) // NOLINT
    {
        auto class_factory = factory_.get_class_factory(CLSID_NetPbmDecoder);

        auto outer = reinterpret_cast<IUnknown*>(1);
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        const hresult result{class_factory->CreateInstance(outer, IID_PPV_ARGS(decoder.put()))};

        Assert::AreEqual(error_no_aggregation, result);
    }

private:
    factory factory_;
};
