#pragma once
#include "Process/Utf8ConsoleProcess.h"

class TestConsoleHandler : public Terminal::ITerminalHandler {
public:
    void SetStdIn(Process::IStdIn* input) override;
    void Print(const std::wstring_view string) override;
    void LineFeed(Terminal::LineFeedMode mode) override;
    void CarriageReturn() override;

    const std::wstring& GetStr() const;

private:
    bool done = false;
    Process::IStdIn* input = nullptr;
    bool carriageReturn = false;
    bool lineFeed = false;
    std::wstring str;
};
