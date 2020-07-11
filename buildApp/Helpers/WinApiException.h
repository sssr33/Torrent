#pragma once

#include <stdexcept>
#include <Windows.h>

namespace Helpers
{
    class WinApiException : public std::runtime_error
    {
    public:
        WinApiException();

        DWORD GetErrorCode() const;

    private:
        DWORD errorCode = ERROR_SUCCESS;
    };
}
