#pragma once
#include "IStdIn.h"
#include "Pipe.h"

namespace Process
{
    class PipeStdIn final : public IStdIn
    {
    public:
        PipeStdIn(Pipe& pipe);

        Arg::BytesWritten Write(const void* data, size_t size) override;

    private:
        Pipe& pipe;
    };
}
