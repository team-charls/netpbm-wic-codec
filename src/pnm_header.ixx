// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

export module pnm_header;

import errors;
import buffered_stream_reader;

using winrt::throw_hresult;
using winrt::check_hresult;

enum class PnmType
{
    Bitmap,
    Graymap,
    Pixmap
};

export bool is_pnm_file(IStream* stream)
{
    char magic[2];

    unsigned long read;
    check_hresult(stream->Read(magic, sizeof magic, &read), wincodec::error_stream_read);

    return read == sizeof magic && magic[0] == 'P' && magic[1] == '5';
}

export struct pnm_header
{
    PnmType PnmType;
    bool AsciiFormat;
    uint32_t ImageWidth;
    uint32_t ImageHeight;
    USHORT MaxColorValue;

    pnm_header() = default;

    pnm_header(buffered_stream_reader& streamReader)
    {
        check_hresult(Parse(streamReader));
    }

    HRESULT Parse(buffered_stream_reader& streamReader)
    {
        char magic[2];
        ULONG bytesRead;

        HRESULT hr = streamReader.ReadBytes((BYTE*)&magic, sizeof(magic), &bytesRead);

        if (FAILED(hr))
            return hr;
        if (bytesRead != sizeof(magic))
            throw_hresult(wincodec::error_bad_header);

        if (magic[0] != 'P' || magic[1] < '1' || magic[1] > '6')
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
        }

        int width;
        int height;

        hr = streamReader.ReadIntSlow(&width);

        if (FAILED(hr))
            return hr;
        if (hr == S_FALSE)
            return WINCODEC_ERR_BADHEADER;

        hr = streamReader.ReadIntSlow(&height);

        if (FAILED(hr))
            return hr;
        if (hr == S_FALSE)
            return WINCODEC_ERR_BADHEADER;

        if (width < 1 || height < 1)
            return WINCODEC_ERR_BADHEADER;

        ImageWidth = width;
        ImageHeight = height;

        int maxColorValue;

        if (PnmType != PnmType::Bitmap)
        {
            hr = streamReader.ReadIntSlow(&maxColorValue);

            if (FAILED(hr))
                return hr;
            if (hr == S_FALSE)
                return WINCODEC_ERR_BADHEADER;
        }
        else
            maxColorValue = 1;

        if (maxColorValue < 1 || maxColorValue > 65535)
            return WINCODEC_ERR_BADHEADER;

        MaxColorValue = maxColorValue;

        return error_ok;
    }
};
