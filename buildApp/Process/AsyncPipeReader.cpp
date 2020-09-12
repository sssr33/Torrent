#include "AsyncPipeReader.h"
#include "Helpers/WinApiException.h"

namespace Process
{
    AsyncPipeReader::AsyncPipeReader(Pipe& pipe, size_t bufferSize)
        : pipe(pipe)
        , buffer(bufferSize)
        , readEvent(Arg::ManualReset(true), Arg::InitialSet(false))
        , overlapped(this->MakeOverlapped())
    {}

    HANDLE AsyncPipeReader::GetReadEvent() const
    {
        return this->readEvent.GetEventHandle();
    }

    const void* AsyncPipeReader::GetBuffer() const
    {
        return this->buffer.data();
    }

    void AsyncPipeReader::BeginRead()
    {
        this->overlapped = this->MakeOverlapped();

        if (!ReadFile(this->pipe.GetReadHandle(), this->buffer.data(), static_cast<DWORD>(this->buffer.size()), nullptr, &this->overlapped))
        {
            DWORD err = GetLastError();

            if (err != ERROR_IO_PENDING)
            {
                throw Helpers::WinApiException();
            }
        }
    }

    Arg::BytesRead AsyncPipeReader::EndRead()
    {
        DWORD bytesRead = 0;

        if (!GetOverlappedResult(this->pipe.GetReadHandle(), &this->overlapped, &bytesRead, TRUE))
        {
            throw Helpers::WinApiException();
        }

        return Arg::BytesRead(bytesRead);
    }

    OVERLAPPED AsyncPipeReader::MakeOverlapped() const
    {
        OVERLAPPED res = {};

        res.hEvent = this->readEvent.GetEventHandle();

        return res;
    }
}
