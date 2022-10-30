// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT
module;

#include <Windows.h>
#include <wincodec.h>
#include "winrt.h"

export module factory;

export inline constexpr GUID CLSID_NetPbmDecoder{0x6891bbe, 0xcc02, 0x4bb2, {0x9c, 0xf0, 0x30, 0x3f, 0xc4, 0xe6, 0x68, 0xc3}};

export class factory final
{
public:
    factory() noexcept(false) : library_{LoadLibrary(L"netpbm-wic-codec.dll")}
    {
        if (!library_)
            winrt::throw_last_error();
    }

    ~factory()
    {
        if (library_)
        {
            FreeLibrary(library_);
        }
    }

    factory(const factory&) = delete;
    factory(factory&&) = delete;
    factory& operator=(const factory&) = delete;
    factory& operator=(factory&&) = delete;

    [[nodiscard]] winrt::com_ptr<IWICBitmapDecoder> create_decoder() const
    {
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::check_hresult(get_class_factory(CLSID_NetPbmDecoder)->CreateInstance(nullptr, IID_PPV_ARGS(decoder.put())));

        return decoder;
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
