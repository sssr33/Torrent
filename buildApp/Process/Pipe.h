#pragma once
#include "Helpers/WinApiDeleters.h"

#include <memory>

namespace Process
{
    class Pipe
    {
    public:
        Pipe();

        HANDLE GetReadHandle() const;
        HANDLE GetWriteHandle() const;

    private:
        std::unique_ptr<Helpers::HandleVal, Helpers::HandleCloseDeleter> readPipe;
        std::unique_ptr<Helpers::HandleVal, Helpers::HandleCloseDeleter> writePipe;
    };
}
