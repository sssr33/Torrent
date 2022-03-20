#include "ProcessTask.h"
#include "ProcessInformation.h"
#include "Pipe.h"
#include "AsyncPipeReader.h"
#include "PipeStdIn.h"
#include "ProcessJobScope.h"
#include "ProcessHelpers.h"
#include "PseudoConsole.h"
#include "Helpers/WinApiException.h"
#include "Helpers/WinApiEvent.h"
#include "Helpers/Scope.h"

#include <Windows.h>
#include <vector>

/*
* https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
* https://docs.microsoft.com/en-us/windows/console/setconsolemode
* https://devblogs.microsoft.com/commandline/windows-command-line-introducing-the-windows-pseudo-console-conpty/
* https://github.com/microsoft/terminal
* https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session
*/

namespace Process
{
    void ProcessTask::Run(const ProcessTaskParameters& params, IProcessTaskHandler& handler)
    {
        ProcessInformation procInfo;

        std::vector<wchar_t> cmdVec(params.commandLine.begin(), params.commandLine.end());
        cmdVec.push_back(0);

        Pipe inputPipe(Arg::Async(true));
        Pipe outputPipe(Arg::Async(true));

        inputPipe.SetReadHandleInheritable(true);
        outputPipe.SetWriteHandleInheritable(true);

        AsyncPipeReader outputReader(outputPipe, 4096);

        PipeStdIn pipeStdIn(inputPipe);

        {
            PseudoConsole pseudoConsole(inputPipe.GetReadHandle(), outputPipe.GetWriteHandle());
            auto startupInfo = pseudoConsole.GetStartupInfoForConsole();

            DWORD procFlags = EXTENDED_STARTUPINFO_PRESENT;

            if (!CreateProcessW(
                params.exePath.empty() ? nullptr : params.exePath.data(),
                cmdVec.data(),
                nullptr,
                nullptr,
                FALSE, // handles are inherited
                procFlags,
                nullptr,
                nullptr,
                &startupInfo.StartupInfo,
                Helpers::GetAddressOf(procInfo)))
            {
                throw Helpers::WinApiException();
            }

            outputReader.BeginRead();

            HANDLE events[] =
            {
                outputReader.GetReadEvent(),
                procInfo.GetProcessHandle()
            };

            handler.SetStdIn(&pipeStdIn);
            auto handlerStdInScope = MakeScope([&handler]()
                {
                    handler.SetStdIn(nullptr);
                });

            // reading and waiting other process
            while (true)
            {
                const DWORD EventsCount = static_cast<DWORD>(std::size(events));
                DWORD status = WaitForMultipleObjectsEx(EventsCount, events, FALSE, INFINITE, TRUE);
                DWORD evtIdx = status - WAIT_OBJECT_0;

                if (evtIdx < EventsCount)
                {
                    if (events[evtIdx] == outputReader.GetReadEvent())
                    {
                        Arg::BytesRead bytesRead = outputReader.EndRead();
                        const void* buffer = outputReader.GetBuffer();

                        handler.OnOutput(buffer, bytesRead);

                        outputReader.BeginRead();
                    }
                    else if (events[evtIdx] == procInfo.GetProcessHandle())
                    {
                        // process exit
                        break;
                    }
                    else
                    {
                        throw std::exception("Unknown event");
                    }
                }
                else
                {
                    switch (status)
                    {
                    case WAIT_FAILED:
                        throw Helpers::WinApiException();
                    case WAIT_TIMEOUT:
                        throw std::exception("Timeout");
                    default:
                        throw std::exception("Unknown status");
                    }
                }
            }
        }

        try
        {
            // flush other process console output
            while (true)
            {
                Arg::BytesRead bytesRead = outputReader.EndRead(true);
                const void* buffer = outputReader.GetBuffer();

                handler.OnOutput(buffer, bytesRead);

                outputReader.BeginRead();
            }
        }
        catch (const Helpers::WinApiException& ex)
        {
            // Overlapped I/O event is not in a signaled state
            // this error is ok and means that flush in EndRead doesn't have any more data
            if (ex.GetErrorCode() != 996)
            {
                throw;
            }
        }
    }
}
