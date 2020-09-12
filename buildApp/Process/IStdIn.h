#pragma once
#include "Helpers/Strong.h"

#include <cstdint>

namespace Arg
{
    using BytesWritten = Helpers::Strong<size_t, struct BytesWrittenParameter>;
}

namespace Process
{
    class IStdIn
    {
    public:
        virtual ~IStdIn() = default;

        virtual Arg::BytesWritten Write(const void* data, size_t size) = 0;
    };
}
