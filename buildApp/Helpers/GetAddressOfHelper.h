#pragma once
#include "GetAddressOfTraits.h"

#include <cassert>

namespace Helpers
{
    template<class DstT>
    class GetAddressOfHelper
    {
    public:
        typedef typename GetAddressOfTraits<DstT> Traits;
        typedef typename Traits::SrcT SrcT;

        GetAddressOfHelper() = default;

        GetAddressOfHelper(DstT& dst)
            : valid(true)
            , dst(&dst)
        {}

        GetAddressOfHelper(const GetAddressOfHelper&) = delete;

        GetAddressOfHelper(GetAddressOfHelper&& other)
            : GetAddressOfHelper()
        {
            swap(*this, other);
        }

        ~GetAddressOfHelper()
        {
            if (!this->dst)
            {
                // empty instance
                return;
            }

            if (Traits::IsSrcEmpty(this->src))
            {
                // may be an exception
                // do not reset(and destroy) what <dst> already has
                return;
            }

            Traits::Reset(*this->dst, this->src);
        }

        GetAddressOfHelper& operator=(GetAddressOfHelper other)
        {
            swap(*this, other);
            return *this;
        }

        friend void swap(GetAddressOfHelper& a, GetAddressOfHelper& b)
        {
            using std::swap;
            swap(a.valid, b.valid);
            swap(a.dst, b.dst);
            swap(a.src, b.src);
        }

        operator SrcT* ()
        {
            assert(Traits::IsSrcEmpty(this->src));
            return &this->src;
        }

        operator SrcT& ()
        {
            assert(Traits::IsSrcEmpty(this->src));
            return this->src;
        }

    private:
        bool valid = false;
        DstT* dst = nullptr;
        SrcT src = Traits::GetEmptySrc();
    };
}
