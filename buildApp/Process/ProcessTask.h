#pragma once

#include <string>

namespace Process
{
    class ProcessTask
    {
    public:
        ProcessTask(std::string exeName);

    private:
        std::string exeName;
    };
}
