#include "ProcessTask.h"
#include "ProcessInformation.h"
#include "Pipe.h"
#include "AsyncPipeReader.h"
#include "PipeStdIn.h"
#include "ProcessJobScope.h"
#include "ProcessHelpers.h"
#include "Helpers/WinApiException.h"
#include "Helpers/WinApiEvent.h"

#include <Windows.h>
#include <vector>

/*
* https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
* https://docs.microsoft.com/en-us/windows/console/setconsolemode
* https://devblogs.microsoft.com/commandline/windows-command-line-introducing-the-windows-pseudo-console-conpty/
* https://github.com/microsoft/terminal
* https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session
*/

// Initializes the specified startup info struct with the required properties and
// updates its thread attribute list with the specified ConPTY handle
HRESULT InitializeStartupInfoAttachedToPseudoConsole(STARTUPINFOEXW* pStartupInfo, HPCON hPC)
{
	HRESULT hr{ E_UNEXPECTED };

	if (pStartupInfo)
	{
		size_t attrListSize{};

		pStartupInfo->StartupInfo.cb = sizeof(STARTUPINFOEXW);

		// Get the size of the thread attribute list.
		InitializeProcThreadAttributeList(NULL, 1, 0, &attrListSize);

		// Allocate a thread attribute list of the correct size
		pStartupInfo->lpAttributeList =
			reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(malloc(attrListSize));

		// Initialize thread attribute list
		if (pStartupInfo->lpAttributeList
			&& InitializeProcThreadAttributeList(pStartupInfo->lpAttributeList, 1, 0, &attrListSize))
		{
			// Set Pseudo Console attribute
			hr = UpdateProcThreadAttribute(
				pStartupInfo->lpAttributeList,
				0,
				PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
				hPC,
				sizeof(HPCON),
				NULL,
				NULL)
				? S_OK
				: HRESULT_FROM_WIN32(GetLastError());
		}
		else
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
	}
	return hr;
}


namespace Process
{
	void ProcessTask::Run(const ProcessTaskParameters& params, IProcessTaskHandler& handler)
	{
		ProcessInformation procInfo;

		std::vector<wchar_t> cmdVec(params.commandLine.begin(), params.commandLine.end());
		cmdVec.push_back(0);

		Pipe inputPipe(Arg::Async(true));
		Pipe outputPipe(Arg::Async(true));
		//Pipe errorPipe(Arg::Async(true));

		inputPipe.SetReadHandleInheritable(true);
		outputPipe.SetWriteHandleInheritable(true);
		//errorPipe.SetWriteHandleInheritable(true);

		AsyncPipeReader outputReader(outputPipe, 4096);
		//AsyncPipeReader errorReader(errorPipe, 4096);

		HRESULT hr = S_OK;
		HPCON hPC{ INVALID_HANDLE_VALUE };

		{
			// Determine required size of Pseudo Console
			COORD consoleSize{};
			CONSOLE_SCREEN_BUFFER_INFO csbi{};
			HANDLE hConsole{ GetStdHandle(STD_OUTPUT_HANDLE) };
			if (GetConsoleScreenBufferInfo(hConsole, &csbi))
			{
				consoleSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
				consoleSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
			}

			// Create the Pseudo Console of the required size, attached to the PTY-end of the pipes
			hr = CreatePseudoConsole(consoleSize, inputPipe.GetReadHandle(), outputPipe.GetWriteHandle(), 0, &hPC);
		}

		bool isOk = hr == S_OK;

		STARTUPINFOEXW startupInfo = {};

		isOk = InitializeStartupInfoAttachedToPseudoConsole(&startupInfo, hPC) == S_OK;

		//startupInfo.cb = sizeof startupInfo;

		//// process reads pipe to get input
		//startupInfo.hStdInput = inputPipe.GetReadHandle();
		//// process writes pipe to give output
		//startupInfo.hStdOutput = outputPipe.GetWriteHandle();
		//// process writes pipe to give errors
		//startupInfo.hStdError = errorPipe.GetWriteHandle();
		//startupInfo.dwFlags |= STARTF_USESTDHANDLES;

		//ProcessJobScope procScope;

		DWORD procFlags = 0; //CREATE_NO_WINDOW
		// https://stackoverflow.com/questions/89588/assignprocesstojobobject-fails-with-access-denied-error-when-running-under-the
		//procFlags |= ProcessHelpers::IsThisProcessInJob() ? CREATE_BREAKAWAY_FROM_JOB : 0;

		procFlags |= EXTENDED_STARTUPINFO_PRESENT;

		if (!CreateProcessW(
			params.exePath.empty() ? nullptr : params.exePath.data(),
			cmdVec.data(),
			nullptr,
			nullptr,
			FALSE, // handles are inherited
			procFlags,//CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&startupInfo.StartupInfo,
			Helpers::GetAddressOf(procInfo)))
		{
			throw Helpers::WinApiException();
		}

		//try
		//{
		//	procScope.AddProcessToJob(procInfo.GetProcessHandle());
		//}
		//catch (const Helpers::WinApiException& /*ex*/)
		//{
		//	// maybe it's not critical
		//	// but it's reccomended to add process to the job to finish it with this process
		//}

		outputReader.BeginRead();
		//errorReader.BeginRead();

		HANDLE events[] =
		{
			outputReader.GetReadEvent(),
			//errorReader.GetReadEvent(),
			procInfo.GetProcessHandle()
		};

		PipeStdIn pipeStdIn(inputPipe);

		while (true)
		{
			const DWORD EventsCount = static_cast<DWORD>(std::size(events));
			DWORD status = WaitForMultipleObjects(EventsCount, events, FALSE, INFINITE);
			DWORD evtIdx = status - WAIT_OBJECT_0;

			if (evtIdx < EventsCount)
			{
				if (events[evtIdx] == outputReader.GetReadEvent())
				{
					Arg::BytesRead bytesRead = outputReader.EndRead();
					const void* buffer = outputReader.GetBuffer();

					handler.OnOutput(buffer, bytesRead, pipeStdIn);

					outputReader.BeginRead();
				}
				/*else if (events[evtIdx] == errorReader.GetReadEvent())
				{
					Arg::BytesRead bytesRead = errorReader.EndRead();
					const void* buffer = errorReader.GetBuffer();

					handler.OnError(buffer, bytesRead, pipeStdIn);

					errorReader.BeginRead();
				}*/
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

		// TODO check if read out all errors and output needed ???
	}
}
