#include "ProcessInformation.h"

namespace Process
{
	ProcessInformation::ProcessInformation(PROCESS_INFORMATION&& procInfo)
		: processHandle(procInfo.hProcess)
		, threadHandle(procInfo.hThread)
		, dwProcessId(procInfo.dwProcessId)
		, dwThreadId(procInfo.dwThreadId)
	{
		procInfo = {};
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
