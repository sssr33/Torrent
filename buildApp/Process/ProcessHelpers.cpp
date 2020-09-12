#include "ProcessHelpers.h"
#include "Helpers/WinApiException.h"

namespace Process
{
    bool ProcessHelpers::IsThisProcessInJob()
    {
        BOOL procInJob = FALSE;

        if (!IsProcessInJob(GetCurrentProcess(), NULL, &procInJob)) {
            throw Helpers::WinApiException();
        }

        return procInJob ? true : false;
    }
}
