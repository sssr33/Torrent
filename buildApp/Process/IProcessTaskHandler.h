#pragma once
#include "IStdIn.h"

namespace Process
{
    class IProcessTaskHandler
    {
    public:
        virtual ~IProcessTaskHandler() = default;

        virtual void OnOutput(const void* data, size_t size, IStdIn& stdIn) = 0;
        virtual void OnError(const void* data, size_t size, IStdIn& stdIn) = 0;
    };
}
