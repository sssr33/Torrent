#pragma once
#include "Pipe.h"
#include "Helpers/Strong.h"
#include "Helpers/WinApiEvent.h"

#include <vector>

namespace Arg
{
    using BytesRead = Helpers::Strong<size_t, struct BytesReadParameter>;
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
        Arg::BytesRead EndRead(bool flush = false);

    private:
        Pipe& pipe;
        std::vector<uint8_t> buffer;
        Helpers::WinApiEvent readEvent;
        OVERLAPPED overlapped = {};

        OVERLAPPED MakeOverlapped() const;
    };
}
