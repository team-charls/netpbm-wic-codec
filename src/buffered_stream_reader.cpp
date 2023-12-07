// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module buffered_stream_reader;

import <std.h>;
import <win.h>;
import winrt;

import errors;
import util;

constexpr UINT MAX_BUFFER_SIZE = 65536;


buffered_stream_reader::buffered_stream_reader(_In_ IStream* stream) : buffer_(MAX_BUFFER_SIZE)
{
    stream_.copy_from(stream);

    // buffer_ = new BYTE[MAX_BUFFER_SIZE];

    unsigned long read;
    check_hresult(stream->Read(buffer_.data(), MAX_BUFFER_SIZE, &read), wincodec::error_stream_read);

    buffer_size_ = read;
}

HRESULT buffered_stream_reader::ReadChar(char* c)
{
    if (position_ + sizeof(char) > buffer_size_)
        if (buffer_size_ == MAX_BUFFER_SIZE)
        {
            RefillBuffer();

            if (buffer_size_ < sizeof(char))
                return S_FALSE;
        }
        else
            return S_FALSE;

    *c = buffer_[position_];

    position_ += sizeof(char);

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

uint32_t buffered_stream_reader::read_int_slow()
{
    char str[12];

    winrt::check_hresult(ReadString(str, sizeof(str)));
    uint32_t value;
    if (const auto [ptr, ec] = std::from_chars(str, std::end(str), value); ec != std::errc())
        winrt::throw_hresult(WINCODEC_ERR_BADSTREAMDATA);

    return value;
}

bool buffered_stream_reader::try_read_bytes(void* buffer, const size_t size)
{
    ULONG bytes_read;
    if (const HRESULT hr{ReadBytes(buffer, static_cast<ULONG>(size), &bytes_read)}; FAILED(hr) || bytes_read != size)
        return false;

    return true;
}

void buffered_stream_reader::read_bytes(void* buffer, size_t size)
{
    const size_t remaining_in_buffer = buffer_size_ - position_;

    if (remaining_in_buffer >= size)
    {
        memcpy(buffer, buffer_.data() + position_, size);
        position_ += static_cast<UINT>(size);
        return;
    }

    memcpy(buffer, buffer_.data() + position_, remaining_in_buffer);
    position_ += static_cast<UINT>(remaining_in_buffer);
    size -= remaining_in_buffer;

    unsigned long read;
    check_hresult(stream_->Read(static_cast<std::byte*>(buffer) + remaining_in_buffer, static_cast<ULONG>(size), &read),
                  wincodec::error_stream_read);
}

HRESULT buffered_stream_reader::ReadBytes(void* buf, const ULONG count, ULONG* bytesRead)
{
    auto b{static_cast<BYTE*>(buf)};
    ULONG remaining = count;

    while (true)
    {
        if (buffer_size_ - position_ >= remaining)
        {
            memcpy(b, buffer_.data() + position_, remaining);
            position_ += remaining;
            *bytesRead = count;
            return S_OK;
        }

        memcpy(b, buffer_.data() + position_, buffer_size_ - position_);
        b += buffer_size_ - position_;
        remaining -= static_cast<ULONG>(buffer_size_ - position_);
        position_ = buffer_size_;

        RefillBuffer();

        if (buffer_size_ == 0)
        {
            *bytesRead = count - remaining;
            return S_OK;
        }
    }
}

void buffered_stream_reader::RefillBuffer()
{
    memcpy(buffer_.data(), buffer_.data() + position_, buffer_size_ - position_);

    position_ = buffer_size_ - position_;

    unsigned long read;
    check_hresult(stream_->Read(buffer_.data() + position_, static_cast<ULONG>(buffer_size_ - position_), &read),
                  wincodec::error_stream_read);

    buffer_size_ = position_ + read;
    position_ = 0;
}
