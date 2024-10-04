// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "intellisense.hpp"
#include "util.hpp"

import std;
import "win.h";
import test.winrt;

import buffered_stream_reader;

using std::span;
using winrt::com_ptr;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(buffered_stream_reader_test)
{
public:
    TEST_METHOD(read_bytes) // NOLINT
    {
        std::vector<char> source;
        source.push_back(0);
        source.push_back(1);
        buffered_stream_reader reader(create_memory_stream(source).get());

        std::vector<char> destination(2);
        ULONG bytes_read;
        reader.read_bytes(destination.data(), static_cast<ULONG>(destination.size()), &bytes_read);

        Assert::AreEqual(2UL, bytes_read);
    }

    TEST_METHOD(read_bytes_not_enough_available) // NOLINT
    {
        std::vector<char> source;
        source.push_back(0);
        buffered_stream_reader reader(create_memory_stream(source).get());

        std::vector<char> destination(2);
        ULONG bytes_read;
        reader.read_bytes(destination.data(), static_cast<ULONG>(destination.size()), &bytes_read);

        Assert::AreEqual(1UL, bytes_read);
    }

    TEST_METHOD(read_int) // NOLINT
    {
        std::vector<char> source;
        std::string str1 = "256 ";
        source.insert(source.end(), str1.begin(), str1.end());
        buffered_stream_reader reader(create_memory_stream(source).get());

        auto value = reader.read_int();

        Assert::AreEqual(256U, value);
    }

private:
    com_ptr<IStream> create_memory_stream(span<char> source)
    {
        com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(reinterpret_cast<BYTE*>(source.data()), static_cast<UINT>(source.size())));

        return stream;
    }
};
