#pragma once
#include "GetAddressOfTraits.h"

#include <memory>

namespace Helpers
{
    template<class Val>
    struct GetAddressOfPtrTraitsBase
    {
        typedef Val* SrcT;

        static bool IsSrcEmpty(const Val* src)
        {
            return src == nullptr;
        }

        static Val* GetEmptySrc()
        {
            return nullptr;
        }
    };

    template<class T, class Val>
    struct GetAddressOfPtrTraitsStdResetMethod
    {
        static void Reset(T& dest, Val* ptr)
        {
            dest.reset(ptr);
        }
    };

    template<class Val, class Del>
    struct GetAddressOfTraits<std::unique_ptr<Val, Del>>
        : public GetAddressOfPtrTraitsBase<Val>
        , public GetAddressOfPtrTraitsStdResetMethod<std::unique_ptr<Val, Del>, Val>
    {};

    template<class Val>
    struct GetAddressOfTraits<std::shared_ptr<Val>>
        : public GetAddressOfPtrTraitsBase<Val>
        , public GetAddressOfPtrTraitsStdResetMethod<std::shared_ptr<Val>, Val>
    {};

    template<class Val>
    struct GetAddressOfTraits<Val*> : public GetAddressOfPtrTraitsBase<Val>
    {
        static void Reset(Val*& dst, typename GetAddressOfPtrTraitsBase<Val>::SrcT src)
        {
            dst = src;
        }
    };
}
