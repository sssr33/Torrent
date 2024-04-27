#include "HResultException.h"

namespace Helpers
{
    HResultException::HResultException(HRESULT hr)
        : std::runtime_error("HResultException")
        , hr(hr)
    {}

    HRESULT HResultException::GetHResult() const
    {
        return hr;
    }

    void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw HResultException(hr);
        }
    }
}
