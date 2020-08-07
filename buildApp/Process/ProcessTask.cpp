#include "ProcessTask.h"
#include "ProcessInformation.h"
#include "Pipe.h"
#include "Helpers/WinApiException.h"
#include "Helpers/WinApiEvent.h"

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

		Pipe inputPipe(Arg::Async(true));
		Pipe outputPipe(Arg::Async(true));
		Pipe errorPipe(Arg::Async(true));

		Helpers::WinApiEvent outputReadEvent(Arg::ManualReset(true), Arg::InitialSet(false));
		Helpers::WinApiEvent errorReadEvent(Arg::ManualReset(true), Arg::InitialSet(false));

		inputPipe.SetReadHandleInheritable(true);
		outputPipe.SetWriteHandleInheritable(true);
		errorPipe.SetWriteHandleInheritable(true);

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
		startupInfo.hStdError = errorPipe.GetWriteHandle();
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

		OVERLAPPED outputOverlapped = {};
		OVERLAPPED errorOverlapped = {};

		outputOverlapped.hEvent = outputReadEvent.GetEventHandle();
		errorOverlapped.hEvent = errorReadEvent.GetEventHandle();

		std::vector<char> outputReadBuf;
		std::vector<char> errorReadBuf;

		outputReadBuf.resize(4096);
		errorReadBuf.resize(4096);

		if (!ReadFile(outputPipe.GetReadHandle(), outputReadBuf.data(), static_cast<DWORD>(outputReadBuf.size()), nullptr, &outputOverlapped))
		{
			DWORD err = GetLastError();

			if (err != ERROR_IO_PENDING)
			{
				throw Helpers::WinApiException();
			}
		}

		if (!ReadFile(errorPipe.GetReadHandle(), errorReadBuf.data(), static_cast<DWORD>(errorReadBuf.size()), nullptr, &errorOverlapped))
		{
			DWORD err = GetLastError();

			if (err != ERROR_IO_PENDING)
			{
				throw Helpers::WinApiException();
			}
		}

		HANDLE events[] =
		{
			errorReadEvent.GetEventHandle(),
			outputReadEvent.GetEventHandle(),
			procInfo.GetProcessHandle()
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
				else if (events[evtIdx] == outputReadEvent.GetEventHandle())
				{
					DWORD bytesRead = 0;
					if (!GetOverlappedResult(outputPipe.GetReadHandle(), &outputOverlapped, &bytesRead, TRUE))
					{
						throw Helpers::WinApiException();
					}

					outputOverlapped = {};
					outputOverlapped.hEvent = outputReadEvent.GetEventHandle();

					if (!ReadFile(outputPipe.GetReadHandle(), outputReadBuf.data(), static_cast<DWORD>(outputReadBuf.size()), nullptr, &outputOverlapped))
					{
						DWORD err = GetLastError();

						if (err != ERROR_IO_PENDING)
						{
							throw Helpers::WinApiException();
						}
					}

					int stop = 234;
				}
				else if (events[evtIdx] == errorReadEvent.GetEventHandle())
				{
					DWORD bytesRead = 0;
					if (!GetOverlappedResult(errorPipe.GetReadHandle(), &errorOverlapped, &bytesRead, TRUE))
					{
						throw Helpers::WinApiException();
					}

					errorOverlapped = {};
					errorOverlapped.hEvent = errorReadEvent.GetEventHandle();

					if (!ReadFile(errorPipe.GetReadHandle(), errorReadBuf.data(), static_cast<DWORD>(errorReadBuf.size()), nullptr, &errorOverlapped))
					{
						DWORD err = GetLastError();

						if (err != ERROR_IO_PENDING)
						{
							throw Helpers::WinApiException();
						}
					}

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


		/*HANDLE events2[] =
		{
			errorReadEvent.GetEventHandle(),
			outputReadEvent.GetEventHandle(),
		};

		const DWORD eventsCount2 = static_cast<DWORD>(std::size(events2));

		while (true)
		{
			DWORD status = WaitForMultipleObjects(eventsCount2, events2, FALSE, 0);
			DWORD evtIdx = status - WAIT_OBJECT_0;

			if (status == WAIT_TIMEOUT)
			{
				break;
			}

			if (evtIdx < eventsCount2)
			{
				if (events2[evtIdx] == outputReadEvent.GetEventHandle())
				{
					DWORD bytesRead = 0;
					if (!GetOverlappedResult(outputPipe.GetReadHandle(), &outputOverlapped, &bytesRead, TRUE))
					{
						throw Helpers::WinApiException();
					}

					outputOverlapped = {};
					outputOverlapped.hEvent = outputReadEvent.GetEventHandle();

					if (!ReadFile(outputPipe.GetReadHandle(), outputReadBuf.data(), outputReadBuf.size(), nullptr, &outputOverlapped))
					{
						DWORD err = GetLastError();

						if (err != ERROR_IO_PENDING)
						{
							throw Helpers::WinApiException();
						}
					}

					int stop = 234;
				}
				else if (events2[evtIdx] == errorReadEvent.GetEventHandle())
				{
					DWORD bytesRead = 0;
					if (!GetOverlappedResult(errorPipe.GetReadHandle(), &errorOverlapped, &bytesRead, TRUE))
					{
						throw Helpers::WinApiException();
					}

					errorOverlapped = {};
					errorOverlapped.hEvent = errorReadEvent.GetEventHandle();

					if (!ReadFile(errorPipe.GetReadHandle(), errorReadBuf.data(), errorReadBuf.size(), nullptr, &errorOverlapped))
					{
						DWORD err = GetLastError();

						if (err != ERROR_IO_PENDING)
						{
							throw Helpers::WinApiException();
						}
					}

					int stop = 234;
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
		}*/

		// read out all errors and output
	}
}
