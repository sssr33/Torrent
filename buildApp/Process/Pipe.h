#pragma once
#include "Helpers/WinApiDeleters.h"

#include <memory>

namespace Process
{
    class Pipe
    {
    public:
        Pipe();

    private:
        std::unique_ptr<Helpers::HandleVal, Helpers::HandleCloseDeleter> readPipe;
        std::unique_ptr<Helpers::HandleVal, Helpers::HandleCloseDeleter> writePipe;
    };
}
