#include "ProcessJobScope.h"
#include "Helpers/WinApiException.h"

namespace Process
{
    ProcessJobScope::ProcessJobScope()
        : job(CreateJobObjectW(nullptr, nullptr))
    {
        if (!this->job)
        {
            throw Helpers::WinApiException();
        }

        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

        if (!SetInformationJobObject(this->job.get(), JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
        {
            throw Helpers::WinApiException();
        }
    }

    void ProcessJobScope::AddProcessToJob(HANDLE process)
    {
        if (!AssignProcessToJobObject(this->job.get(), process))
        {
            throw Helpers::WinApiException();
        }
    }
}
