#pragma once
#include "Pipe.h"
#include "Helpers/Strong.h"
#include "Helpers/WinApiEvent.h"

#include <vector>

namespace Arg
{
    using BytesRead = Helpers::Strong<DWORD, struct BytesReadParameter>;
}

namespace Process
{
    class AsyncPipeReader
    {
    public:
        explicit AsyncPipeReader(Pipe& pipe, size_t bufferSize);
        AsyncPipeReader(const AsyncPipeReader&) = delete;
        AsyncPipeReader(AsyncPipeReader&&) = delete;

        HANDLE GetReadEvent() const;
        const void* GetBuffer() const;

        void BeginRead();
        Arg::BytesRead EndRead();

    private:
        Pipe& pipe;
        std::vector<uint8_t> buffer;
        Helpers::WinApiEvent readEvent;
        OVERLAPPED overlapped = {};

        OVERLAPPED MakeOverlapped() const;
    };
}
