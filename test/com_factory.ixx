// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "intellisense.hpp"

export module com_factory;

import <win.hpp>;
import test.winrt;

export constexpr GUID net_pbm_decoder_class_id{0x6891bbe, 0xcc02, 0x4bb2, {0x9c, 0xf0, 0x30, 0x3f, 0xc4, 0xe6, 0x68, 0xc3}};

/// <summary>
/// Helper class that provides methods to create COM objects without registry registration.
/// </summary>
export class com_factory final
{
public:
    com_factory() noexcept(false) : library_{LoadLibrary(L"netpbm-wic-codec.dll")}
    {
        if (!library_)
            winrt::throw_last_error();
    }

    ~com_factory()
    {
        if (library_)
        {
            FreeLibrary(library_);
        }
    }

    com_factory(const com_factory&) = delete;
    com_factory(com_factory&&) = delete;
    com_factory& operator=(const com_factory&) = delete;
    com_factory& operator=(com_factory&&) = delete;

    [[nodiscard]] winrt::com_ptr<IWICBitmapDecoder> create_decoder() const
    {
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::check_hresult(get_class_factory(net_pbm_decoder_class_id)->CreateInstance(nullptr, IID_PPV_ARGS(decoder.put())));

        return decoder;
    }

    [[nodiscard]] winrt::com_ptr<IPropertyStore> create_property_store() const
    {
        constexpr GUID property_store_class_id{0xab993d53, 0xee9c, 0x496b, 0x8c, 0x8b, 0x30, 0x35, 0x6e, 0x3c, 0x24, 0xc0};

        winrt::com_ptr<IPropertyStore> property_store;
        winrt::check_hresult(
            get_class_factory(property_store_class_id)->CreateInstance(nullptr, IID_PPV_ARGS(property_store.put())));

        return property_store;
    }

    [[nodiscard]] winrt::com_ptr<IClassFactory> get_class_factory(GUID const& class_id) const
    {
        winrt::com_ptr<IClassFactory> class_factory;
        winrt::check_hresult(get_class_factory(class_id, class_factory));

        return class_factory;
    }

    [[nodiscard]] HRESULT get_class_factory(GUID const& class_id, winrt::com_ptr<IClassFactory>& class_factory) const
    {
        const auto get_class_object =
            static_cast<dll_get_class_object_ptr>(static_cast<void*>(GetProcAddress(library_, "DllGetClassObject")));
        if (!get_class_object)
            winrt::throw_last_error();

        return get_class_object(class_id, IID_PPV_ARGS(class_factory.put()));
    }

private:
    using dll_get_class_object_ptr = HRESULT(_stdcall*)(GUID const&, GUID const&, void** result);

    HINSTANCE library_{};
};
