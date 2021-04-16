// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

export module buffered_stream_reader;

export class buffered_stream_reader final
{
public:
    buffered_stream_reader(_In_ IStream* stream);

    HRESULT ReadChar(char* c);
    HRESULT SkipLine();
    HRESULT ReadString(char* str, ULONG maxCount);
    HRESULT ReadIntSlow(int* i);
    bool try_read_bytes(void* buffer, size_t size);
    void read_bytes(void* buffer, size_t size);
    HRESULT ReadBytes(void* buf, ULONG count, ULONG* bytesRead);

private:
    void RefillBuffer();

    winrt::com_ptr<IStream> stream_;
    std::vector<BYTE> buffer_;
    UINT bufferSize{};
    UINT position{};
};
