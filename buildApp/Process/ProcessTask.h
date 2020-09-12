#pragma once
#include "ProcessTaskParameters.h"

#include <string>
#include <memory>

namespace Process
{
    class ProcessTask
    {
    public:
        static void Run(const ProcessTaskParameters& params);
    };
}
