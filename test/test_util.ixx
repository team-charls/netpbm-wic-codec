// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT
module;

#include "winrt.h"

#include <span> // cannot use import as it causes C1001 in VS 2022 17.4 Preview 5.0

export module test_util;

import <shlwapi.h>;


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
