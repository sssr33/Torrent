#pragma once

#include <string>

namespace Process
{
    class ProcessTask
    {
    public:
        explicit ProcessTask(std::wstring exeName);

        void Run();

    private:
        std::wstring exeName;
    };
}
