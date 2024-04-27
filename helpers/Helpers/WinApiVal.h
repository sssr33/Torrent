#pragma once

#include <Windows.h>
#include <type_traits>

namespace Helpers
{
    using HandleVal = std::remove_pointer_t<HANDLE>;
}
