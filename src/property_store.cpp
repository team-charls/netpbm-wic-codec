// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "macros.hpp"

module property_store;

import std;
import <win.hpp>;
import winrt;

import hresults;
import util;
import buffered_stream_reader;
import pnm_header;
import class_factory;
import property_variant;

using std::scoped_lock;
using std::span;
using std::pair;
using std::uint32_t;
using winrt::check_hresult;
using winrt::to_hresult;

namespace {

template<typename K, typename V, std::size_t Extent>
[[nodiscard]] const V* find(span<pair<K, V>, Extent> values, const K& key)
{
    for (const auto& pair : values)
    {
        if (pair.first == key)
        {
            return &pair.second;
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

        scoped_lock lock{mutex_};

        if (initialized_)
            return error_already_initialized;

        buffered_stream_reader stream_reader{stream};
        const pnm_header header{stream_reader};
        properties_[0].first = PKEY_Image_HorizontalSize;
        properties_[0].second = property_variant{header.width};
        properties_[1].first = PKEY_Image_VerticalSize;
        properties_[1].second = property_variant{header.height};
        properties_[2].first = PKEY_Image_BitDepth;
        properties_[2].second = property_variant{compute_bit_depth(header.PnmType, header.MaxColorValue)};
        properties_[3].first = PKEY_Image_Dimensions;
        properties_[3].second = property_variant{std::format(L"{}x{}", header.width, header.height).c_str()};
        properties_[4].first = PKEY_Image_Compression;
        properties_[4].second = property_variant{static_cast<std::uint16_t>(IMAGE_COMPRESSION_UNCOMPRESSED)};
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
        return S_FALSE;
    }

    // IPropertyStore Interface
    HRESULT __stdcall GetCount(DWORD* count) noexcept override
    try
    {
        TRACE("{} property_store::GetCount\n", fmt_ptr(this));
        scoped_lock lock{mutex_};
        *check_out_pointer(count) = initialized_ ? static_cast<DWORD>(properties_.size()) : 0UL;
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
        scoped_lock lock{mutex_};

        if (index >= properties_.size() || !initialized_)
            return error_invalid_argument;

        *check_out_pointer(key) = properties_[index].first;
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
        if (const auto* property_value{find(span{properties_}, key)}; property_value)
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
    std::mutex mutex_;
    std::array<pair<PROPERTYKEY, property_variant>, 5> properties_; // Use std::array as a small map.
    bool initialized_{};
};

} // namespace

void create_property_store_class_factory(GUID const& interface_id, void** result)
{
    check_hresult(winrt::make<class_factory<property_store>>()->QueryInterface(interface_id, result));
}
