// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "intellisense.hpp"

export module buffered_stream_reader;

import <win.hpp>;
import std;
import winrt;

export class buffered_stream_reader final
{
public:
    explicit buffered_stream_reader(_In_ IStream* stream);

    [[nodiscard]] std::uint32_t read_int();
    [[nodiscard]] bool try_read_bytes(void* buffer, size_t size);
    void read_bytes(void* buffer, size_t size);

    [[nodiscard]] std::vector<std::byte> read_bytes(const size_t size)
    {
        std::vector<std::byte> bytes(size);
        read_bytes(bytes.data(), bytes.size());
        return bytes;
    }

    void read_bytes(void* buf, ULONG count, ULONG* bytesRead);

private:
    char read_char();
    void skip_line();
    void read_string(char* str, ULONG maxCount);
    void RefillBuffer();

    winrt::com_ptr<IStream> stream_;
    std::vector<BYTE> buffer_;
    size_t buffer_size_{};
    size_t position_{};
};
