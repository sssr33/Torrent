#include "DefaultTerminalParser.h"
#include "stateMachine.hpp"
#include "OutputStateMachineEngine.hpp"
#include "ITermDispatch.hpp"

#include <cassert>

namespace Terminal {

    class DefaultTermDispatch : public Microsoft::Console::VirtualTerminal::ITermDispatch
    {
    public:
        explicit DefaultTermDispatch(std::shared_ptr<ITerminalHandler> handler)
            : handler(std::move(handler))
        {}

        void Execute(const wchar_t wchControl) override
        {
            int stop = 234;
        }

        void Print(const wchar_t wchPrintable) override
        {
            this->PrintString(std::wstring_view(&wchPrintable, 1));
        }

        void PrintString(const std::wstring_view string) override
        {
            this->handler->Print(string);
        }

        bool CursorUp(const size_t distance) override
        {
            return true;
        }

        bool CursorDown(const size_t distance) override
        {
            return true;
        }

        bool CursorForward(const size_t distance) override
        {
            return true;
        }

        bool CursorBackward(const size_t distance) override
        {
            return true;
        }

        bool CursorNextLine(const size_t distance) override
        {
            return true;
        }

        bool CursorPrevLine(const size_t distance) override
        {
            return true;
        }

        bool CursorHorizontalPositionAbsolute(const size_t column) override
        {
            return true;
        }

        bool VerticalLinePositionAbsolute(const size_t line) override
        {
            return true;
        }

        bool HorizontalPositionRelative(const size_t distance) override
        {
            return true;
        }

        bool VerticalPositionRelative(const size_t distance) override
        {
            return true;
        }

        bool CursorPosition(const size_t line, const size_t column) override
        {
            return true;
        }

        bool CursorSaveState() override
        {
            return true;
        }

        bool CursorRestoreState() override
        {
            return true;
        }

        bool CursorVisibility(const bool isVisible) override
        {
            return true;
        }

        bool InsertCharacter(const size_t count) override
        {
            return true;
        }

        bool DeleteCharacter(const size_t count) override
        {
            return true;
        }

        bool ScrollUp(const size_t distance) override
        {
            return true;
        }

        bool ScrollDown(const size_t distance) override
        {
            return true;
        }

        bool InsertLine(const size_t distance) override
        {
            return true;
        }

        bool DeleteLine(const size_t distance) override
        {
            return true;
        }

        bool SetColumns(const size_t columns) override
        {
            return true;
        }

        bool SetCursorKeysMode(const bool applicationMode) override
        {
            return true;
        }

        bool SetKeypadMode(const bool applicationMode) override
        {
            return true;
        }

        bool EnableWin32InputMode(const bool win32InputMode) override
        {
            return true;
        }

        bool EnableCursorBlinking(const bool enable) override
        {
            return true;
        }

        bool SetAnsiMode(const bool ansiMode) override
        {
            return true;
        }

        bool SetScreenMode(const bool reverseMode) override
        {
            return true;
        }

        bool SetOriginMode(const bool relativeMode) override
        {
            return true;
        }

        bool SetAutoWrapMode(const bool wrapAtEOL) override
        {
            return true;
        }

        bool SetTopBottomScrollingMargins(const size_t topMargin, const size_t bottomMargin) override
        {
            return true;
        }

        bool WarningBell() override
        {
            return true;
        }

        bool CarriageReturn() override
        {
            this->handler->CarriageReturn();
            return true;
        }

        bool LineFeed(const Microsoft::Console::VirtualTerminal::DispatchTypes::LineFeedType lineFeedType) override
        {
            Terminal::LineFeedMode mode = Terminal::LineFeedMode::Unknown;

            switch (lineFeedType)
            {
            case Microsoft::Console::VirtualTerminal::DispatchTypes::LineFeedType::DependsOnMode:
                mode = Terminal::LineFeedMode::DependsOnMode;
                break;
            case Microsoft::Console::VirtualTerminal::DispatchTypes::LineFeedType::WithReturn:
                mode = Terminal::LineFeedMode::WithReturn;
                break;
            case Microsoft::Console::VirtualTerminal::DispatchTypes::LineFeedType::WithoutReturn:
                mode = Terminal::LineFeedMode::NoReturn;
                break;
            }

            this->handler->LineFeed(mode);

            return true;
        }

        bool ReverseLineFeed() override
        {
            return true;
        }

        bool SetWindowTitle(std::wstring_view title) override
        {
            return true;
        }

        bool UseAlternateScreenBuffer() override
        {
            return true;
        }

        bool UseMainScreenBuffer() override
        {
            return true;
        }

        bool HorizontalTabSet() override
        {
            return true;
        }

        bool ForwardTab(const size_t numTabs) override
        {
            return true;
        }

        bool BackwardsTab(const size_t numTabs) override
        {
            return true;
        }

        bool TabClear(const Microsoft::Console::VirtualTerminal::DispatchTypes::TabClearType clearType) override
        {
            return true;
        }

        bool EnableDECCOLMSupport(const bool enabled) override
        {
            return true;
        }

        bool EnableVT200MouseMode(const bool enabled) override
        {
            return true;
        }

        bool EnableUTF8ExtendedMouseMode(const bool enabled) override
        {
            return true;
        }

        bool EnableSGRExtendedMouseMode(const bool enabled) override
        {
            return true;
        }

        bool EnableButtonEventMouseMode(const bool enabled) override
        {
            return true;
        }

        bool EnableAnyEventMouseMode(const bool enabled) override
        {
            return true;
        }

        bool EnableAlternateScroll(const bool enabled) override
        {
            return true;
        }

        bool EnableXtermBracketedPasteMode(const bool enabled) override
        {
            return true;
        }

        bool SetColorTableEntry(const size_t tableIndex, const DWORD color) override
        {
            return true;
        }

        bool SetDefaultForeground(const DWORD color) override
        {
            return true;
        }

        bool SetDefaultBackground(const DWORD color) override
        {
            return true;
        }

        bool EraseInDisplay(const Microsoft::Console::VirtualTerminal::DispatchTypes::EraseType eraseType) override
        {
            return true;
        }

        bool EraseInLine(const Microsoft::Console::VirtualTerminal::DispatchTypes::EraseType eraseType) override
        {
            return true;
        }

        bool EraseCharacters(const size_t numChars) override
        {
            return true;
        }

        bool SetGraphicsRendition(const Microsoft::Console::VirtualTerminal::VTParameters options) override
        {
            return true;
        }

        bool SetLineRendition(const LineRendition rendition) override
        {
            return true;
        }

        bool PushGraphicsRendition(const Microsoft::Console::VirtualTerminal::VTParameters options) override
        {
            return true;
        }

        bool PopGraphicsRendition() override
        {
            return true;
        }

        bool SetMode(const Microsoft::Console::VirtualTerminal::DispatchTypes::ModeParams param) override
        {
            return true;
        }

        bool ResetMode(const Microsoft::Console::VirtualTerminal::DispatchTypes::ModeParams param) override
        {
            return true;
        }

        bool DeviceStatusReport(const Microsoft::Console::VirtualTerminal::DispatchTypes::AnsiStatusType statusType) override
        {
            return true;
        }

        bool DeviceAttributes() override
        {
            return true;
        }

        bool SecondaryDeviceAttributes() override
        {
            return true;
        }

        bool TertiaryDeviceAttributes() override
        {
            return true;
        }

        bool Vt52DeviceAttributes() override
        {
            return true;
        }

        bool RequestTerminalParameters(const Microsoft::Console::VirtualTerminal::DispatchTypes::ReportingPermission permission) override
        {
            return true;
        }

        bool DesignateCodingSystem(const Microsoft::Console::VirtualTerminal::VTID codingSystem) override
        {
            return true;
        }

        bool Designate94Charset(const size_t gsetNumber, const Microsoft::Console::VirtualTerminal::VTID charset) override
        {
            return true;
        }

        bool Designate96Charset(const size_t gsetNumber, const Microsoft::Console::VirtualTerminal::VTID charset) override
        {
            return true;
        }

        bool LockingShift(const size_t gsetNumber) override
        {
            return true;
        }

        bool LockingShiftRight(const size_t gsetNumber) override
        {
            return true;
        }

        bool SingleShift(const size_t gsetNumber) override
        {
            return true;
        }

        bool SoftReset() override
        {
            return true;
        }

        bool HardReset() override
        {
            return true;
        }

        bool ScreenAlignmentPattern() override
        {
            return true;
        }

        bool SetCursorStyle(const Microsoft::Console::VirtualTerminal::DispatchTypes::CursorStyle cursorStyle) override
        {
            return true;
        }

        bool SetCursorColor(const COLORREF color) override
        {
            return true;
        }

        bool SetClipboard(std::wstring_view content) override
        {
            return true;
        }

        // DTTERM_WindowManipulation
        bool WindowManipulation(const Microsoft::Console::VirtualTerminal::DispatchTypes::WindowManipulationType function,
            const Microsoft::Console::VirtualTerminal::VTParameter parameter1,
            const Microsoft::Console::VirtualTerminal::VTParameter parameter2) override
        {
            return true;
        }


        bool AddHyperlink(const std::wstring_view uri, const std::wstring_view params) override
        {
            return true;
        }

        bool EndHyperlink() override
        {
            return true;
        }

        bool DoConEmuAction(const std::wstring_view string) override
        {
            return true;
        }

    private:
        std::shared_ptr<ITerminalHandler> handler;
    };

    class DefaultTerminalParser : public ITerminalParser
    {
    public:
        explicit DefaultTerminalParser(std::shared_ptr<ITerminalHandler> handler)
            : stateMachine(
                std::make_unique<Microsoft::Console::VirtualTerminal::OutputStateMachineEngine>(
                    std::make_unique<DefaultTermDispatch>(std::move(handler))))
        {}

        void Process(const std::wstring_view string) override
        {
            this->stateMachine.ProcessString(string);
        }

    private:
        Microsoft::Console::VirtualTerminal::StateMachine stateMachine;
    };

    std::unique_ptr<ITerminalParser> CreateDefaultTerminalParser(std::shared_ptr<ITerminalHandler> handler)
    {
        return std::make_unique<DefaultTerminalParser>(std::move(handler));
    }
}
