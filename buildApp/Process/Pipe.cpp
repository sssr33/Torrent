#include "Pipe.h"
#include "MyCreatePipeEx.h"
#include "Helpers/GetAddressOf.h"
#include "Helpers/WinApiException.h"

namespace Process
{
    Pipe::Pipe(Arg::Async async)
    {
        DWORD flag = async ? FILE_FLAG_OVERLAPPED : 0;

        if (!MyCreatePipeEx()(
            Helpers::GetAddressOf(this->readPipe),
            Helpers::GetAddressOf(this->writePipe),
            nullptr,
            0,
            flag,
            flag))
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

    bool Pipe::IsReadHandleInheritable() const
    {
        return this->IsHandleInheritable(this->readPipe.get());
    }

    void Pipe::SetReadHandleInheritable(bool inheritable)
    {
        this->SetHandleInheritable(this->readPipe.get(), inheritable);
    }

    bool Pipe::IsWriteHandleInheritable() const
    {
        return this->IsHandleInheritable(this->writePipe.get());
    }

    void Pipe::SetWriteHandleInheritable(bool inheritable)
    {
        this->SetHandleInheritable(this->writePipe.get(), inheritable);
    }

    bool Pipe::IsHandleInheritable(HANDLE handle) const
    {
        DWORD flags = 0;

        if (!GetHandleInformation(handle, &flags))
        {
            throw Helpers::WinApiException();
        }

        bool inheritable = (flags & HANDLE_FLAG_INHERIT) != 0;
        return inheritable;
    }

    void Pipe::SetHandleInheritable(HANDLE handle, bool inheritable)
    {
        DWORD flag = inheritable ? HANDLE_FLAG_INHERIT : 0;

        if (!SetHandleInformation(handle, HANDLE_FLAG_INHERIT, flag))
        {
            throw Helpers::WinApiException();
        }
    }
}
