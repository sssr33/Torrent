#include "Pipe.h"
#include "Helpers/GetAddressOf.h"
#include "Helpers/WinApiException.h"

namespace Process
{
    Pipe::Pipe()
    {
        SECURITY_ATTRIBUTES sec = {};

        sec.nLength = sizeof SECURITY_ATTRIBUTES;
        sec.bInheritHandle = TRUE;

        if (!CreatePipe(
            Helpers::GetAddressOf(this->readPipe),
            Helpers::GetAddressOf(this->writePipe),
            &sec,
            0))
        {
            throw Helpers::WinApiException();
        }
    }

    HANDLE Pipe::GetReadHandle() const
    {
        return this->readPipe.get();
    }

    HANDLE Pipe::GetWriteHandle() const
    {
        return this->writePipe.get();
    }
}
