// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "macros.hpp"
#include "intellisense.hpp"

module buffered_stream_reader;

import <win.hpp>;

import hresults;
import util;

using std::uint32_t;

constexpr UINT MAX_BUFFER_SIZE = 65536;


buffered_stream_reader::buffered_stream_reader(_In_ IStream* stream) : buffer_(MAX_BUFFER_SIZE)
{
    ASSERT(stream);

    stream_.copy_from(stream);

    // buffer_ = new BYTE[MAX_BUFFER_SIZE];

    unsigned long read;
    check_hresult(stream->Read(buffer_.data(), MAX_BUFFER_SIZE, &read), wincodec::error_stream_read);

    buffer_size_ = read;
}

uint32_t buffered_stream_reader::read_int()
{
    char str[12];

    read_string(str, sizeof(str));
    uint32_t value;
    if (const auto [ptr, ec] = std::from_chars(str, std::end(str), value); ec != std::errc())
        winrt::throw_hresult(wincodec::error_bad_header);

    return value;
}

bool buffered_stream_reader::try_read_bytes(void* buffer, const size_t size)
{
    ULONG bytes_read;
    read_bytes(buffer, static_cast<ULONG>(size), &bytes_read);
    if (bytes_read != size)
        return false;

    return true;
}

char buffered_stream_reader::read_char()
{
    if (position_ + sizeof(char) > buffer_size_)
    {
        if (buffer_size_ == MAX_BUFFER_SIZE)
        {
            RefillBuffer();

            if (buffer_size_ < sizeof(char))
                winrt::throw_hresult(wincodec::error_bad_header);
        }
        else
            winrt::throw_hresult(wincodec::error_bad_header);
    }

    const char result = buffer_[position_];

    position_ += sizeof(char);

    return result;
}

void buffered_stream_reader::skip_line()
{
    while (true)
    {
        const char c = read_char();

        if (c == 0x0A)
            return;
    }
}

void buffered_stream_reader::read_string(char* str, ULONG maxCount)
{
    ASSERT(maxCount > 0);

    ULONG charsRead = 0;

    while (true)
    {
        char c = read_char();

        if (isspace(c))
            if (charsRead == 0)
                continue;
            else
            {
                *str = 0x00;
                return;
            }

        if (c == '#')
            if (charsRead == 0)
            {
                skip_line();
                continue;
            }
            else
            {
                winrt::throw_hresult(WINCODEC_ERR_BADSTREAMDATA);
            }

        *str = c;
        str++;
        charsRead++;

        if (charsRead == maxCount)
            winrt::throw_hresult(WINCODEC_ERR_BADSTREAMDATA);
    }
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

void buffered_stream_reader::read_bytes(void* buf, const ULONG count, ULONG* bytesRead)
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
            return;
        }

        memcpy(b, buffer_.data() + position_, buffer_size_ - position_);
        b += buffer_size_ - position_;
        remaining -= static_cast<ULONG>(buffer_size_ - position_);
        position_ = buffer_size_;

        RefillBuffer();

        if (buffer_size_ == 0)
        {
            *bytesRead = count - remaining;
            return;
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
