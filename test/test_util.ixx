// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

export module test.util;

import std;
import <win.hpp>;

import test.winrt;

export winrt::com_ptr<IStream> create_memory_stream(const void* data, const size_t size) noexcept
{
    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream(static_cast<const BYTE*>(data), static_cast<UINT>(size)));

    return stream;
}

export winrt::com_ptr<IStream> create_memory_stream(const std::span<const std::byte> data) noexcept
{
    return create_memory_stream(data.data(), data.size());
}

export constexpr bool failed(const winrt::hresult result) noexcept
{
    return result < 0;
}

export constexpr bool succeeded(winrt::hresult const result) noexcept
{
    return result >= 0;
}
