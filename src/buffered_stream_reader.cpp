module;

#include "pch.h"

module buffered_stream_reader;

import errors;


import errors;

constexpr UINT MAX_BUFFER_SIZE = 65536;


buffered_stream_reader::buffered_stream_reader(_In_ IStream* stream) : buffer_(MAX_BUFFER_SIZE)
{
    stream_.copy_from(stream);

    // buffer_ = new BYTE[MAX_BUFFER_SIZE];

    unsigned long read;
    check_hresult(stream->Read(buffer_.data(), MAX_BUFFER_SIZE, &read), wincodec::error_stream_read);

    bufferSize = read;
}

HRESULT buffered_stream_reader::ReadChar(char* c)
{
    if (position + sizeof(char) > bufferSize)
        if (bufferSize == MAX_BUFFER_SIZE)
        {
            RefillBuffer();

            if (bufferSize < sizeof(char))
                return S_FALSE;
        }
        else
            return S_FALSE;

    *c = buffer_[position];

    position += sizeof(char);

    return S_OK;
}

HRESULT buffered_stream_reader::SkipLine()
{
    while (true)
    {
        char c;

        const HRESULT hr = ReadChar(&c);

        if (hr != S_OK)
            return hr;

        if (c == 0x0A)
            return S_OK;
    }
}

HRESULT buffered_stream_reader::ReadString(char* str, ULONG maxCount)
{
    if (maxCount == 0)
        return E_INVALIDARG;

    ULONG charsRead = 0;

    while (true)
    {
        char c;

        HRESULT hr = ReadChar(&c);

        if (FAILED(hr))
            return hr;

        if (hr == S_FALSE)
            if (charsRead == 0)
                return S_FALSE;
            else
            {
                *str = 0x00;
                return S_OK;
            }

        if (isspace(c))
            if (charsRead == 0)
                continue;
            else
            {
                *str = 0x00;
                return S_OK;
            }

        if (c == '#')
            if (charsRead == 0)
            {
                hr = SkipLine();

                if (FAILED(hr))
                    return hr;

                if (hr == S_FALSE)
                {
                    *str = 0x00;
                    return hr;
                }

                continue;
            }
            else
            {
                *str = 0x00;
                return S_OK;
            }

        *str = c;
        str++;
        charsRead++;

        if (charsRead == maxCount)
            return WINCODEC_ERR_BADSTREAMDATA;
    }
}

HRESULT buffered_stream_reader::ReadIntSlow(int* i)
{
    char str[12];

    const HRESULT hr = ReadString(str, sizeof(str));

    if (hr != S_OK)
        return hr;

    *i = atoi(str);

    return S_OK;
}

bool buffered_stream_reader::try_read_bytes(void* buffer, size_t size)
{
    ULONG bytes_read;
    const HRESULT hr = ReadBytes(buffer, static_cast<ULONG>(size), &bytes_read);
    if (FAILED(hr) || bytes_read != size)
        return false;

    return true;
}

void buffered_stream_reader::read_bytes(void* buffer, size_t size)
{
    size_t remaining_in_buffer = bufferSize - position;

    if (remaining_in_buffer >= size)
    {
        memcpy(buffer, buffer_.data() + position, size);
        position += static_cast<UINT>(size);
        return;
    }

    memcpy(buffer, buffer_.data() + position, remaining_in_buffer);
    position += static_cast<UINT>(remaining_in_buffer);
    size -= remaining_in_buffer;

    unsigned long read;
    check_hresult(stream_->Read(static_cast<std::byte*>(buffer) + remaining_in_buffer, static_cast<ULONG>(size), &read),
                  wincodec::error_stream_read);
}

HRESULT buffered_stream_reader::ReadBytes(void* buf, ULONG count, ULONG* bytesRead)
{
    BYTE* b{static_cast<BYTE*>(buf)};
    ULONG remaining = count;

    while (true)
    {
        if (bufferSize - position >= remaining)
        {
            memcpy(b, buffer_.data() + position, remaining);
            position += remaining;
            *bytesRead = count;
            return S_OK;
        }

        memcpy(b, buffer_.data() + position, bufferSize - position);
        b += bufferSize - position;
        remaining -= bufferSize - position;
        position = bufferSize;

        RefillBuffer();

        if (bufferSize == 0)
        {
            *bytesRead = count - remaining;
            return S_OK;
        }
    }
}

void buffered_stream_reader::RefillBuffer()
{
    memcpy(buffer_.data(), buffer_.data() + position, bufferSize - position);

    position = bufferSize - position;

    unsigned long read;
    check_hresult(stream_->Read(buffer_.data() + position, bufferSize - position, &read), wincodec::error_stream_read);

    bufferSize = position + read;
    position = 0;
}
