// SPDX-FileCopyrightText: © 2024 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "intellisense.hpp"

module property_store;

import std;
import winrt;
import <win.hpp>;

import hresults;
import util;
import buffered_stream_reader;
import pnm_header;
import class_factory;
import property_variant;
import "macros.hpp";

using std::array;
using std::pair;
using std::span;
using std::to_wstring;
using std::uint32_t;
using winrt::check_hresult;

namespace {

template<typename Key, typename Value, size_t Extent>
[[nodiscard]]
const Value* find(span<const Key, Extent> keys, span<Value, Extent> values, const Key& key_to_find)
{
    ASSERT(keys.size() == values.size());

    for (size_t i{}; i < keys.size(); ++i)
    {
        if (keys[i] == key_to_find)
        {
            return &values[i];
        }
    }

    return nullptr;
}

[[nodiscard]] uint32_t compute_bit_depth(const PnmType type, const uint32_t max_value) noexcept
{
    switch (type)
    {
    case PnmType::Bitmap:
        return 2;

    case PnmType::Graymap:
        return std::bit_width(max_value);

    case PnmType::Pixmap:
        return std::bit_width(max_value) * 3;
    }

    std::unreachable();
}


struct property_store : winrt::implements<property_store, IInitializeWithStream, IPropertyStoreCapabilities, IPropertyStore>
{
    // IInitializeWithStream
    HRESULT __stdcall Initialize(_In_ IStream* stream, [[maybe_unused]] const DWORD access_mode) noexcept override
    try
    {
        TRACE("{} property_store::Initialize, stream address={}, access_mode={}\n", fmt_ptr(this), fmt_ptr(stream),
              static_cast<int>(access_mode));

        if (initialized_)
            return error_already_initialized;

        buffered_stream_reader stream_reader{stream};
        const pnm_header header{stream_reader};

        property_values_[0] = property_variant{header.width};
        property_values_[1] = property_variant{header.height};
        property_values_[2] = property_variant{compute_bit_depth(header.PnmType, header.MaxColorValue)};
        property_values_[3] = property_variant{(to_wstring(header.width) + L" x " + to_wstring(header.height)).c_str()};
        property_values_[4] = property_variant{static_cast<std::uint16_t>(IMAGE_COMPRESSION_UNCOMPRESSED)};

        initialized_ = true;

        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    // IPropertyStoreCapabilities
    HRESULT __stdcall IsPropertyWritable(const PROPERTYKEY& /*key*/) noexcept override
    {
        TRACE("{} property_store::IsPropertyWritable\n", fmt_ptr(this));
        return success_false;
    }

    // IPropertyStore Interface
    HRESULT __stdcall GetCount(DWORD* count) noexcept override
    try
    {
        TRACE("{} property_store::GetCount\n", fmt_ptr(this));

        // Implementation recommendation is to set count to zero (when possible) in an error condition.
        *check_out_pointer(count) = static_cast<DWORD>(initialized_ ? property_keys.size() : 0U);
        check_state();

        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetAt(const DWORD index, PROPERTYKEY* key) noexcept override
    try
    {
        TRACE("{} property_store::GetAt\n", fmt_ptr(this));
        check_state();

        if (index >= property_keys.size())
            return error_invalid_argument;

        *check_out_pointer(key) = property_keys[index];
        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall GetValue(const PROPERTYKEY& key, PROPVARIANT* value) noexcept override
    try
    {
        TRACE("{} property_store::GetValue\n", fmt_ptr(this));
        check_state();


        if (const auto* property_value{find(span{property_keys}, span{property_values_}, key)}; property_value)
        {
            property_value->copy(value);
        }
        else
        {
            PropVariantInit(check_out_pointer(value));
        }

        return success_ok;
    }
    catch (...)
    {
        return to_hresult();
    }

    HRESULT __stdcall SetValue(const PROPERTYKEY& /*key*/, const PROPVARIANT& /*value*/) noexcept override
    {
        TRACE("{} property_store::SetValue\n", fmt_ptr(this));
        return error_access_denied;
    }

    HRESULT __stdcall Commit() noexcept override
    {
        TRACE("{} property_store::Commit\n", fmt_ptr(this));
        return error_access_denied;
    }

private:
    void check_state() const
    {
        if (!initialized_)
            winrt::throw_hresult(error_not_valid_state);
    }

    inline const static array property_keys{PKEY_Image_HorizontalSize, PKEY_Image_VerticalSize, PKEY_Image_BitDepth,
                                            PKEY_Image_Dimensions, PKEY_Image_Compression};
    array<property_variant, property_keys.size()> property_values_;
    std::atomic<bool> initialized_{};
};

} // namespace

void create_property_store_class_factory(GUID const& interface_id, void** result)
{
    check_hresult(winrt::make<class_factory<property_store>>()->QueryInterface(interface_id, result));
}
