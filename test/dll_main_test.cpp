// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "util.hpp"

import std;
import <win.h>;
import test.winrt;
import test.errors;
import factory;

using namespace winrt;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// {358bb60e-2b77-4411-8a76-27164945c93d}
constexpr GUID random_class_id{0x358bb60e, 0x2b77, 0x4411, {0x8a, 0x76, 0x27, 0x16, 0x49, 0x45, 0xc9, 0x3d}};

TEST_CLASS(dllmain_test)
{
public:
    TEST_METHOD(class_factory_netpbm_decoder_lock_server) // NOLINT
    {
        const auto class_factory{factory_.get_class_factory(net_pbm_decoder_class_id)};

        auto result{class_factory->LockServer(true)};
        Assert::AreEqual(error_ok, result);

        result = class_factory->LockServer(false);
        Assert::AreEqual(error_ok, result);
    }

    TEST_METHOD(class_factory_unknown_id) // NOLINT
    {
        com_ptr<IClassFactory> class_factory;
        const auto result{factory_.get_class_factory(random_class_id, class_factory)};

        Assert::AreEqual(error_class_not_available, result);
    }

    TEST_METHOD(class_factory_netpbm_decoder_create_instance_bad_result) // NOLINT
    {
        const auto class_factory{factory_.get_class_factory(net_pbm_decoder_class_id)};

        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const auto result{class_factory->CreateInstance(nullptr, random_class_id, nullptr)};

        Assert::AreEqual(error_pointer, result);
    }

    TEST_METHOD(class_factory_netpbm_decoder_create_instance_no_aggregation) // NOLINT
    {
        const auto class_factory{factory_.get_class_factory(net_pbm_decoder_class_id)};

        auto outer = reinterpret_cast<IUnknown*>(1);
        com_ptr<IWICBitmapDecoder> decoder;
        const auto result{class_factory->CreateInstance(outer, IID_PPV_ARGS(decoder.put()))};

        Assert::AreEqual(error_no_aggregation, result);
    }

    TEST_METHOD(marked_for_self_registration) // NOLINT
    {
        const wchar_t dll_name[]{L"netpbm-wic-codec.dll"};
        DWORD not_used;
        const DWORD size{GetFileVersionInfoSizeEx(FILE_VER_GET_NEUTRAL, dll_name, &not_used)};
        Assert::IsTrue(size != 0);

        std::vector<std::byte> buffer(size);
        bool result = GetFileVersionInfoEx(FILE_VER_GET_NEUTRAL, dll_name, 0, size, buffer.data());
        Assert::IsTrue(result);

        void* value;
        UINT value_size;
        result = VerQueryValue(buffer.data(), L"\\StringFileInfo\\000004b0\\OLESelfRegister", &value, &value_size);
        Assert::IsTrue(result);
    }

private:
    factory factory_;
};
