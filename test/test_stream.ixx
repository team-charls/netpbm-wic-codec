// SPDX-FileCopyrightText: © 2020 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "intellisense.hpp"

export module test.stream;

import <win.hpp>;

import test.winrt;
import test.hresults;

export struct test_stream : winrt::implements<test_stream, IStream>
{
    test_stream(const bool fail_on_read, const int fail_on_seek_counter) noexcept :
        fail_on_read_{fail_on_read}, fail_on_seek_counter_{fail_on_seek_counter}
    {
    }

    HRESULT __stdcall Read(_Out_writes_bytes_to_(cb, *pcbRead) void* /*pv*/, [[maybe_unused]] _In_ ULONG cb,
                           _Out_opt_ ULONG* pcbRead) noexcept override
    {
        if (pcbRead)
            *pcbRead = 0;

        return fail_on_read_ ? error_fail : success_ok;
    }

    HRESULT __stdcall Write(_In_reads_bytes_(cb) const void* /*pv*/, [[maybe_unused]] _In_ ULONG cb,
                            _Out_opt_ ULONG* /*pcbWritten*/) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall Seek(LARGE_INTEGER, DWORD /*dwOrigin*/, _Out_opt_ ULARGE_INTEGER* libNewPosition) override
    {
        --fail_on_seek_counter_;
        if (libNewPosition)
            libNewPosition->QuadPart = 0L;

        return fail_on_seek_counter_ <= 0 ? error_fail : success_ok;
    }

    HRESULT __stdcall SetSize(ULARGE_INTEGER /*libNewSize*/) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall CopyTo(_In_ IStream*, ULARGE_INTEGER /*cb*/, _Out_opt_ ULARGE_INTEGER* /*pcbRead*/,
                             _Out_opt_ ULARGE_INTEGER* /*pcbWritten*/) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall Commit(DWORD /*grfCommitFlags*/) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall Revert() override
    {
        return error_fail;
    }

    HRESULT __stdcall LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/,
                                   DWORD /*dwLockType*/) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall Stat(__RPC__out STATSTG*, DWORD /*grfStatFlag*/) noexcept override
    {
        return error_fail;
    }

    HRESULT __stdcall Clone(__RPC__deref_out_opt IStream**) noexcept override
    {
        return error_fail;
    }

private:
    bool fail_on_read_;
    int fail_on_seek_counter_;
};
