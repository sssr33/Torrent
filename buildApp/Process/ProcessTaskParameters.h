#pragma once

#include <string>

namespace Process
{
    struct ProcessTaskParameters
    {
        std::wstring exePath;
        std::wstring commandLine;
    };
}
