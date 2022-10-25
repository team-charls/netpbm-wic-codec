// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT
module;

#include "winrt.h"
#include <shlwapi.h>

#include <span>

export module test_util;

export winrt::com_ptr<IStream> create_memory_stream(const void* data, const size_t size) noexcept
{
    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream(static_cast<const BYTE*>(data), static_cast<UINT>(size)));

    return stream;
}

export winrt::com_ptr<IStream> create_memory_stream(std::span<std::byte> data) noexcept
{
    return create_memory_stream(data.data(), data.size());
}
