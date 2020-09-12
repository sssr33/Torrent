#pragma once
#include "Helpers/WinApiSmartHandle.h"

namespace Process
{
    // https://stackoverflow.com/questions/24012773/c-winapi-how-to-kill-child-processes-when-the-calling-parent-process-is-for/24020820
    class ProcessJobScope
    {
    public:
        ProcessJobScope();

        void AddProcessToJob(HANDLE process);

    private:
        Helpers::UniqueCloseHandle job;
    };
}
