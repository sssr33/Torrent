#include "PipeStdIn.h"
#include "Helpers/WinApiException.h"

namespace Process
{
    PipeStdIn::PipeStdIn(Pipe& pipe)
        : pipe(pipe)
    {}

    Arg::BytesWritten PipeStdIn::Write(const void* data, size_t size)
    {
        HANDLE writeHandle = this->pipe.GetWriteHandle();
        DWORD bytesWritten = 0;

        if (!WriteFile(writeHandle, data, static_cast<DWORD>(size), &bytesWritten, nullptr))
        {
            DWORD err = GetLastError();

            if (err != ERROR_IO_PENDING)
            {
                throw Helpers::WinApiException();
            }
        }

        return Arg::BytesWritten(static_cast<size_t>(bytesWritten));
    }
}
