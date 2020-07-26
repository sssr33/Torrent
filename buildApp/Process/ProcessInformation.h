#pragma once
#include "Helpers/WinApiSmartHandle.h"
#include "Helpers/GetAddressOf.h"

#include <memory>
#include <Windows.h>

namespace Process
{
	class ProcessInformation
	{
	public:
		ProcessInformation() = default;
		explicit ProcessInformation(PROCESS_INFORMATION&& procInfo);

		HANDLE GetProcessHandle() const;
		HANDLE GetThreadHandle() const;
		DWORD GetProcessId() const;
		DWORD GetThreadId() const;

	private:
		Helpers::UniqueCloseHandle processHandle;
		Helpers::UniqueCloseHandle threadHandle;
		DWORD processId = 0;
		DWORD threadId = 0;
	};
}

namespace Helpers
{
	template<>
    struct GetAddressOfTraits<Process::ProcessInformation>
    {
        typedef PROCESS_INFORMATION SrcT;

        static void Reset(Process::ProcessInformation& dst, PROCESS_INFORMATION& src);
		static bool IsSrcEmpty(const PROCESS_INFORMATION& src);
		static PROCESS_INFORMATION GetEmptySrc();
    };
}
