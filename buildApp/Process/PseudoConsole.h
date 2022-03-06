#pragma once

#include "Helpers/WinApiSmartHandle.h"

#include <vector>

namespace Process
{
    class PseudoConsole
    {
    public:
        PseudoConsole(HANDLE read, HANDLE write);
        PseudoConsole(const PseudoConsole&) = delete;
        PseudoConsole(PseudoConsole&&) = default;
        ~PseudoConsole();

        PseudoConsole& operator=(const PseudoConsole&) = delete;
        PseudoConsole& operator=(PseudoConsole&&) = default;

        STARTUPINFOEXW GetStartupInfoForConsole() const;

    private:
        using HPCONVal = std::remove_pointer_t<HPCON>;

        struct PseudoConsoleCloseDeleter
        {
            void operator()(HPCONVal* handle);
        };

        using PseudoConsoleCloseHandle = std::unique_ptr<HPCONVal, PseudoConsoleCloseDeleter>;

        // Initializes the specified startup info struct with the required properties and
        // updates its thread attribute list with the specified ConPTY handle
        class StartupInfo
        {
        public:
            explicit StartupInfo(HPCON hPC);
            StartupInfo(const StartupInfo&) = delete;
            StartupInfo(StartupInfo&&) = default;
            ~StartupInfo();

            StartupInfo& operator=(const StartupInfo&) = delete;
            StartupInfo& operator=(StartupInfo&&) = default;

            STARTUPINFOEXW GetStartupInfoForConsole() const;

        private:
            STARTUPINFOEXW info = {};
            std::vector<uint8_t> attrList;
        };

        static PseudoConsoleCloseHandle MakePseudoConsole(HANDLE read, HANDLE write);

        PseudoConsoleCloseHandle console;
        StartupInfo startupInfo;
    };
}
