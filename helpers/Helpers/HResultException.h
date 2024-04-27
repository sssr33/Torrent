#pragma once

#include <stdexcept>
#include <Windows.h>

namespace Helpers
{
    class HResultException : public std::runtime_error
    {
    public:
        explicit HResultException(HRESULT hr);

        HRESULT GetHResult() const;

    private:
        HRESULT hr = S_OK;
    };

    void ThrowIfFailed(HRESULT hr);
}
