#pragma once
#include "GetAddressOfHelper.h"
#include "GetAddressOfPtrTraits.h"

//#include <memory>
//#include <cassert>

namespace Helpers
{
    //template<class T>
    //struct GetAddressOfTraits
    //{
    //    static void Reset(T& dest, T src)
    //    {
    //        static_assert(false, "not implemented")
    //    }
    //};

    //template<class T, class Val>
    //struct GetAddressOfTraitsStdResetMethod
    //{
    //    typedef Val Val;

    //    static void Reset(T& dest, Val* ptr)
    //    {
    //        dest.reset(ptr);
    //    }
    //};

    //template<class Val, class Del>
    //struct GetAddressOfTraits<std::unique_ptr<Val, Del>> : public GetAddressOfTraitsStdResetMethod<std::unique_ptr<Val, Del>, Val> {};

    //template<class Val>
    //struct GetAddressOfTraits<std::shared_ptr<Val>> : public GetAddressOfTraitsStdResetMethod<std::shared_ptr<Val>, Val> {};

    //template<class Val>
    //struct GetAddressOfTraits<Val*>
    //{
    //    typedef Val Val;

    //    static void Reset(Val*& dest, Val* ptr)
    //    {
    //        dest = ptr;
    //    }
    //};

    //template<class PtrT>
    //class GetAddressOfHelper
    //{
    //public:
    //    typedef typename GetAddressOfTraits<PtrT>::Val Val;

    //    GetAddressOfHelper(PtrT& ptr)
    //        : ptr(&ptr)
    //    {}

    //    GetAddressOfHelper(const GetAddressOfHelper&) = delete;

    //    GetAddressOfHelper(GetAddressOfHelper&& other)
    //        : GetAddressOfHelper()
    //    {
    //        swap(*this, other);
    //    }

    //    ~GetAddressOfHelper()
    //    {
    //        if (!this->ptr)
    //        {
    //            // empty helper
    //            return;
    //        }

    //        if (!this->tmpPtr)
    //        {
    //            // may be an exception
    //            // do not reset(and destroy) what <ptr> already has
    //            return;
    //        }

    //        GetAddressOfTraits<PtrT>::Reset(*this->ptr, this->tmpPtr);
    //    }

    //    GetAddressOfHelper& operator=(GetAddressOfHelper other)
    //    {
    //        swap(*this, other);
    //        return *this;
    //    }

    //    friend void swap(GetAddressOfHelper& a, GetAddressOfHelper& b)
    //    {
    //        using std::swap;
    //        swap(a.tmpPtr, b.tmpPtr);
    //        swap(a.ptr, b.ptr);
    //    }

    //    operator Val** ()
    //    {
    //        assert(tmpPtr == nullptr);
    //        return &this->tmpPtr;
    //    }

    //private:
    //    Val* tmpPtr = nullptr;
    //    PtrT* ptr = nullptr;
    //};

    template<class PtrT>
    GetAddressOfHelper<PtrT> GetAddressOf(PtrT& ptr)
    {
        return GetAddressOfHelper<PtrT>(ptr);
    }
}
