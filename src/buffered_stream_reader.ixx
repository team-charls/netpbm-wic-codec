// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

export module buffered_stream_reader;

import errors;

export class buffered_stream_reader final
{
public:
    buffered_stream_reader(IStream* stream)
    {
        stream_.copy_from(stream);

        buffer = new BYTE[MAX_BUFFER_SIZE];

        unsigned long read;
        check_hresult(stream->Read(buffer, MAX_BUFFER_SIZE, &read), wincodec::error_stream_not_available);

        bufferSize = read;
    }

    ~buffered_stream_reader()
    {
        delete[] buffer;
    }

    HRESULT ReadChar(char* c)
    {
        if (position + sizeof(char) > bufferSize)
            if (bufferSize == MAX_BUFFER_SIZE)
            {
                HRESULT hr = RefillBuffer();

                if (FAILED(hr))
                    return hr;

                if (bufferSize < sizeof(char))
                    return S_FALSE;
            }
            else
                return S_FALSE;

        *c = buffer[position];

        position += sizeof(char);

        return S_OK;
    }

    HRESULT SkipLine()
    {
        while (true)
        {
            char c;

            HRESULT hr = ReadChar(&c);

            if (hr != S_OK)
                return hr;

            if (c == 0x0A)
                return S_OK;
        }
    }

    HRESULT ReadString(char* str, ULONG maxCount)
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

    //HRESULT ReadInt(int* i);

    HRESULT ReadIntSlow(int* i)
    {
        char str[12];

        HRESULT hr = ReadString(str, sizeof(str));

        if (hr != S_OK)
            return hr;

        *i = atoi(str);

        return S_OK;
    }

    bool try_read_bytes(void* buffer, size_t size)
    {
        ULONG bytes_read;
        HRESULT hr = ReadBytes(buffer, static_cast<ULONG>(size), &bytes_read);
        if (FAILED(hr) || bytes_read != size)
            return false;

        return true;
    }

    HRESULT ReadBytes(void* buf, ULONG count, ULONG* bytesRead)
    {
        BYTE* b{static_cast<BYTE*>(buf)};
        ULONG remaining = count;

        while (true)
        {
            if (bufferSize - position >= remaining)
            {
                memcpy(b, buffer + position, remaining);
                position += remaining;
                *bytesRead = count;
                return S_OK;
            }

            memcpy(b, buffer + position, bufferSize - position);
            b += bufferSize - position;
            remaining -= bufferSize - position;
            position = bufferSize;

            HRESULT hr = RefillBuffer();

            if (FAILED(hr))
                return hr;

            if (bufferSize == 0)
            {
                *bytesRead = count - remaining;
                return S_OK;
            }
        }
    }

    //HRESULT GetPosition(ULARGE_INTEGER* streamPosition) const;
    //HRESULT Seek(LARGE_INTEGER move, DWORD origin);

private:
    HRESULT RefillBuffer()
    {
        memcpy(buffer, buffer + position, bufferSize - position);

        position = bufferSize - position;

        ULONG read;

        HRESULT hr = stream_->Read(buffer + position, bufferSize - position, &read);

        if (FAILED(hr))
            return WINCODEC_ERR_STREAMREAD;

        bufferSize = position + read;
        position = 0;

        return hr;
    }


    winrt::com_ptr<IStream> stream_;
    BYTE* buffer{};
    UINT bufferSize{};
    UINT position{};

    static const UINT MAX_BUFFER_SIZE = 65536;
};
