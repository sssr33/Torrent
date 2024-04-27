#pragma once
#include "WinApiVal.h"

namespace Helpers
{
    struct HandleCloseDeleter
    {
        void operator()(HandleVal* handle);
    };
}
