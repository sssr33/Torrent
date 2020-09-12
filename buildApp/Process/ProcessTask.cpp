#include "ProcessTask.h"
#include "ProcessInformation.h"
#include "Pipe.h"
#include "AsyncPipeReader.h"
#include "ProcessJobScope.h"
#include "ProcessHelpers.h"
#include "Helpers/WinApiException.h"
#include "Helpers/WinApiEvent.h"

#include <Windows.h>
#include <vector>

namespace Process
{
	void ProcessTask::Run(const ProcessTaskParameters& params)
	{
		STARTUPINFOW startupInfo = {};
		ProcessInformation procInfo;

		std::vector<wchar_t> cmdVec(params.commandLine.begin(), params.commandLine.end());
		cmdVec.push_back(0);

		startupInfo.cb = sizeof startupInfo;

		Pipe inputPipe(Arg::Async(true));
		Pipe outputPipe(Arg::Async(true));
		Pipe errorPipe(Arg::Async(true));

		inputPipe.SetReadHandleInheritable(true);
		outputPipe.SetWriteHandleInheritable(true);
		errorPipe.SetWriteHandleInheritable(true);

		bool ir = inputPipe.IsReadHandleInheritable();
		bool iw = inputPipe.IsWriteHandleInheritable();

		bool or = outputPipe.IsReadHandleInheritable();
		bool ow = outputPipe.IsWriteHandleInheritable();

		bool er = errorPipe.IsReadHandleInheritable();
		bool ew = errorPipe.IsWriteHandleInheritable();

		AsyncPipeReader outputReader(outputPipe, 4096);
		AsyncPipeReader errorReader(errorPipe, 4096);

		// process reads pipe to get input
		startupInfo.hStdInput = inputPipe.GetReadHandle();
		// process writes pipe to give output
		startupInfo.hStdOutput = outputPipe.GetWriteHandle();
		// process writes pipe to give errors
		startupInfo.hStdError = errorPipe.GetWriteHandle();
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;

		ProcessJobScope procScope;

		DWORD procFlags = 0; //CREATE_NO_WINDOW
		// https://stackoverflow.com/questions/89588/assignprocesstojobobject-fails-with-access-denied-error-when-running-under-the
		procFlags |= ProcessHelpers::IsThisProcessInJob() ? CREATE_BREAKAWAY_FROM_JOB : 0;

		if (!CreateProcessW(
			params.exePath.empty() ? nullptr : params.exePath.data(),
			cmdVec.data(),
			nullptr,
			nullptr,
			TRUE, // handles are inherited
			procFlags,//CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&startupInfo,
			Helpers::GetAddressOf(procInfo)))
		{
			throw Helpers::WinApiException();
		}

		try
		{
			procScope.AddProcessToJob(procInfo.GetProcessHandle());
		}
		catch (const Helpers::WinApiException& /*ex*/)
		{
			// maybe it's not critical
			// but it's reccomended to add process to the job to finish it with this process
		}

		outputReader.BeginRead();
		errorReader.BeginRead();

		HANDLE events[] =
		{
			outputReader.GetReadEvent(),
			errorReader.GetReadEvent(),
			procInfo.GetProcessHandle()
		};

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

					// process data

					outputReader.BeginRead();

					int stop = 234;
				}
				else if (events[evtIdx] == errorReader.GetReadEvent())
				{
					Arg::BytesRead bytesRead = errorReader.EndRead();
					const void* buffer = errorReader.GetBuffer();

					// process data

					errorReader.BeginRead();

					int stop = 234;
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

		// TODO check if read out all errors and output needed ???
	}
}
