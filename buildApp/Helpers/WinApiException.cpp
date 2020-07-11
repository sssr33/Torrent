#include "WinApiException.h"

namespace Helpers
{
    WinApiException::WinApiException()
        : std::runtime_error("WinApiException")
        , errorCode(GetLastError())
    {}

    DWORD WinApiException::GetErrorCode() const
    {
        return this->errorCode;
    }
}
