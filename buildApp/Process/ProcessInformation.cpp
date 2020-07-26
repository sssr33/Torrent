#include "ProcessInformation.h"

namespace Process
{
	ProcessInformation::ProcessInformation(PROCESS_INFORMATION&& procInfo)
		: processHandle(procInfo.hProcess)
		, threadHandle(procInfo.hThread)
		, processId(procInfo.dwProcessId)
		, threadId(procInfo.dwThreadId)
	{
		procInfo = {};
	}

	HANDLE ProcessInformation::GetProcessHandle() const
	{
		return this->processHandle.get();
	}

	HANDLE ProcessInformation::GetThreadHandle() const
	{
		return this->threadHandle.get();
	}

	DWORD ProcessInformation::GetProcessId() const
	{
		return this->processId;
	}

	DWORD ProcessInformation::GetThreadId() const
	{
		return this->threadId;
	}
}

namespace Helpers
{
	void GetAddressOfTraits<Process::ProcessInformation>::Reset(Process::ProcessInformation& dst, PROCESS_INFORMATION& src)
	{
		dst = Process::ProcessInformation(std::move(src));
	}

	bool GetAddressOfTraits<Process::ProcessInformation>::IsSrcEmpty(const PROCESS_INFORMATION& src)
	{
		bool empty = src.hProcess == nullptr
			&& src.hThread == nullptr
			&& src.dwProcessId == 0
			&& src.dwThreadId == 0
			;

		return empty;
	}

	PROCESS_INFORMATION GetAddressOfTraits<Process::ProcessInformation>::GetEmptySrc()
	{
		PROCESS_INFORMATION empty = {};
		return empty;
	}
}
