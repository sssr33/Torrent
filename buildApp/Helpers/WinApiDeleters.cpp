#include "WinApiDeleters.h"

namespace Helpers
{
    void HandleCloseDeleter::operator()(HandleVal* handle)
    {
        CloseHandle(handle);
    }
}
