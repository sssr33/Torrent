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

	private:
		Helpers::UniqueCloseHandle processHandle;
		Helpers::UniqueCloseHandle threadHandle;
		DWORD dwProcessId = 0;
		DWORD dwThreadId = 0;
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
