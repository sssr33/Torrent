#pragma once
#include "GetAddressOfHelper.h"
#include "GetAddressOfPtrTraits.h"

namespace Helpers
{
    template<class PtrT>
    GetAddressOfHelper<PtrT> GetAddressOf(PtrT& ptr)
    {
        return GetAddressOfHelper<PtrT>(ptr);
    }
}
