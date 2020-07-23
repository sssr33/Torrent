#include "ProcessTask.h"
#include "ProcessInformation.h"
#include "Helpers/WinApiException.h"

#include <Windows.h>

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

		if (!CreateProcessW(
			this->exeName.c_str(),
			cmdLine,
			nullptr,
			nullptr,
			FALSE,
			CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&startupInfo,
			Helpers::GetAddressOf(procInfo)))
		{
			throw Helpers::WinApiException();
		}
	}
}
