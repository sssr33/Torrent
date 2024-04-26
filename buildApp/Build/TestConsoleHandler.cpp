#include "TestConsoleHandler.h"

void TestConsoleHandler::SetStdIn(Process::IStdIn* input) {
    this->input = input;
}

void TestConsoleHandler::Print(const std::wstring_view string) {
    if (this->carriageReturn && !this->lineFeed) {
        while (!this->str.empty() && this->str.back() != '\r' && this->str.back() != '\n') {
            this->str.pop_back();
        }
    }

    this->str += string;

    if (!this->done && this->str.find(L"0x213") != std::wstring::npos) {
        done = true;

        std::string str = "echo 00xx\r\n";

        this->input->Write(str.data(), str.size());
    }

    this->carriageReturn = false;
    this->lineFeed = false;
}

void TestConsoleHandler::LineFeed(Terminal::LineFeedMode mode) {
    this->str += L"\r\n";
    this->lineFeed = true;
}

void TestConsoleHandler::CarriageReturn() {
    this->carriageReturn = true;
}

const std::wstring& TestConsoleHandler::GetStr() const {
    return this->str;
}
