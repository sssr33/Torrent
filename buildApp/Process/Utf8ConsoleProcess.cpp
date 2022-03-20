#include "Utf8ConsoleProcess.h"
#include "Helpers/Utf.h"

namespace Process
{
    void Utf8ConsoleProcess::Run(const ProcessTaskParameters& params, std::shared_ptr<Terminal::ITerminalHandler> handler)
    {
        ProcessHandler procHandler(std::move(handler));

        Process::ProcessTask::Run(params, procHandler);
    }

    Utf8ConsoleProcess::ProcessHandler::ProcessHandler(std::shared_ptr<Terminal::ITerminalHandler> handler)
        : terminalParser(Terminal::CreateDefaultTerminalParser(handler))
        , handler(handler)
    {}

    void Utf8ConsoleProcess::ProcessHandler::SetStdIn(IStdIn* input)
    {
        this->handler->SetStdIn(input);
    }

    void Utf8ConsoleProcess::ProcessHandler::OnOutput(const void* data, size_t size)
    {
        auto inputData = std::string(static_cast<const char*>(data), static_cast<const char*>(data) + size);
        auto w16Str = Helpers::Utf::Cvt<std::wstring>(inputData);

        this->terminalParser->Process(w16Str);
    }

    void Utf8ConsoleProcess::ProcessHandler::OnError(const void* data, size_t size)
    {
        throw std::exception("Utf8ConsoleProcess error");
    }
}
