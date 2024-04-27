#pragma once

namespace Helpers
{
    template<class DstT>
    struct GetAddressOfTraits
    {
        // <SrcT> is the type from which type <DstT> can be constructed/initialized/reset
        typedef void* SrcT;

        // constructs/initializes/resets <DstT> from <SrcT>
        static void Reset(DstT& dst, SrcT& src)
        {
            static_assert(false, "not implemented");
        }

        // checks if <SrcT> empty/initialized
        static bool IsSrcEmpty(const SrcT& src)
        {
            static_assert(false, "not implemented");
        }

        static SrcT GetEmptySrc()
        {
            static_assert(false, "not implemented");
        }
    };
}
