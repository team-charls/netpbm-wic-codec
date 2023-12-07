// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

export module buffered_stream_reader;

import <win.h>;
import <std.h>;
import winrt;

export class buffered_stream_reader final
{
public:
    explicit buffered_stream_reader(_In_ IStream* stream);

    HRESULT ReadChar(char* c);
    HRESULT SkipLine();
    HRESULT ReadString(char* str, ULONG maxCount);
    [[nodiscard]] uint32_t read_int_slow();
    bool try_read_bytes(void* buffer, size_t size);
    void read_bytes(void* buffer, size_t size);

    [[nodiscard]] std::vector<std::byte> read_bytes(const size_t size)
    {
        std::vector<std::byte> bytes(size);
        read_bytes(bytes.data(), bytes.size());
        return bytes;
    }

    HRESULT ReadBytes(void* buf, ULONG count, ULONG* bytesRead);

private:
    void RefillBuffer();

    winrt::com_ptr<IStream> stream_;
    std::vector<BYTE> buffer_;
    size_t buffer_size_{};
    size_t position_{};
};
