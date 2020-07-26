#include "ProcessTask.h"
#include "ProcessInformation.h"
#include "Pipe.h"
#include "Helpers/WinApiException.h"

#include <Windows.h>
#include <vector>

namespace Process
{
	ProcessTask::ProcessTask(std::wstring exeName)
		: exeName(std::move(exeName))
	{}

	void ProcessTask::Run()
	{
		wchar_t* cmdLine = nullptr;

		STARTUPINFOW startupInfo = {};
		ProcessInformation procInfo;

		std::wstring cmd = L"ping google.com";
		std::vector<wchar_t> cmdVec(cmd.begin(), cmd.end());
		cmdVec.push_back(0);

		startupInfo.cb = sizeof startupInfo;

		Pipe inputPipe;
		Pipe outputPipe;
		Pipe errorPipe;

		/*inputPipe.SetReadHandleInheritable(true);
		outputPipe.SetWriteHandleInheritable(true);
		errorPipe.SetWriteHandleInheritable(true);*/

		bool ir = inputPipe.IsReadHandleInheritable();
		bool iw = inputPipe.IsWriteHandleInheritable();

		bool or = outputPipe.IsReadHandleInheritable();
		bool ow = outputPipe.IsWriteHandleInheritable();

		bool er = errorPipe.IsReadHandleInheritable();
		bool ew = errorPipe.IsWriteHandleInheritable();

		// process reads pipe to get input
		startupInfo.hStdInput = inputPipe.GetReadHandle();
		// process writes pipe to give output
		startupInfo.hStdOutput = outputPipe.GetWriteHandle();
		// process writes pipe to give errors
		//startupInfo.hStdError = errorPipe.GetWriteHandle();
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;

		if (!CreateProcessW(
			nullptr,//this->exeName.c_str(),
			cmdVec.data(),// cmdLine,
			nullptr,
			nullptr,
			TRUE, // handles are inherited
			0,//CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&startupInfo,
			Helpers::GetAddressOf(procInfo)))
		{
			throw Helpers::WinApiException();
		}

		HANDLE events[] =
		{
			inputPipe.GetReadHandle(),
			outputPipe.GetWriteHandle(),
			//errorPipe.GetWriteHandle(),
			//procInfo.GetProcessHandle()
		};

		const DWORD eventsCount = static_cast<DWORD>(std::size(events));

		while (true)
		{
			DWORD status = WaitForMultipleObjects(eventsCount, events, FALSE, INFINITE);
			DWORD evtIdx = status - WAIT_OBJECT_0;

			if (evtIdx < eventsCount)
			{
				if (events[evtIdx] == inputPipe.GetReadHandle())
				{
					int stop = 234; // input ?
				}
				else if (events[evtIdx] == outputPipe.GetWriteHandle())
				{
					int stop = 234;
				}
				else if (events[evtIdx] == errorPipe.GetWriteHandle())
				{
					int stop = 234;
				}
				else if (events[evtIdx] == procInfo.GetProcessHandle())
				{
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

		// read out all errors and output
	}
}
