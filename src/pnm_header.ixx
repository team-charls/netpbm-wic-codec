// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "intellisense.hpp"

export module pnm_header;

import std;
import <win.hpp>;
import winrt;

import buffered_stream_reader;
import hresults;
import util;

using winrt::throw_hresult;
using std::uint32_t;

export enum class PnmType
{
    Bitmap,
    Graymap,
    Pixmap
};

export bool is_pnm_file(_In_ IStream* stream)
{
    char magic[2];

    unsigned long read;
    check_hresult(stream->Read(magic, sizeof magic, &read), wincodec::error_stream_read);

    return read == sizeof magic && magic[0] == 'P' && (magic[1] == '5' || magic[1] == '6');
}

export struct pnm_header
{
    PnmType PnmType;
    bool AsciiFormat;
    uint32_t width;
    uint32_t height;
    USHORT MaxColorValue;

    pnm_header() = default;

    explicit pnm_header(buffered_stream_reader& streamReader)
    {
        winrt::check_hresult(Parse(streamReader));
    }

    HRESULT Parse(buffered_stream_reader& streamReader)
    {
        char magic[2];
        ULONG bytesRead;

        streamReader.read_bytes(&magic, sizeof(magic), &bytesRead);
        if (bytesRead != sizeof(magic))
            throw_hresult(wincodec::error_bad_header);

        if (magic[0] != 'P')
            throw_hresult(wincodec::error_bad_header);

        AsciiFormat = false;

        switch (magic[1])
        {
        case '1': // P1: bitmap, ASCII
            AsciiFormat = true;
        case '4': // P4: bitmap, binary
            PnmType = PnmType::Bitmap;
            break;
        case '2': // P2: graymap, ASCII
            AsciiFormat = true;
        case '5': // P5: graymap, binary
            PnmType = PnmType::Graymap;
            break;
        case '3': // P3: pixmap, ASCII
            AsciiFormat = true;
        case '6': // P6: pixmap, binary
            PnmType = PnmType::Pixmap;
            break;
        default:
            throw_hresult(wincodec::error_bad_header);
        }

        width = streamReader.read_int();
        height = streamReader.read_int();

        if (width < 1 || height < 1)
            return wincodec::error_bad_header;

        int maxColorValue;

        if (PnmType != PnmType::Bitmap)
        {
            maxColorValue = streamReader.read_int();
        }
        else
            maxColorValue = 1;

        if (maxColorValue < 1 || maxColorValue > 65535)
            return WINCODEC_ERR_BADHEADER;

        MaxColorValue = static_cast<USHORT>(maxColorValue);

        return success_ok;
    }
};
