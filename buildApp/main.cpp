#include "BuildAppConfig.h"
#include "Process/Pipe.h"
#include "Process/ProcessInformation.h"

#include <iostream>

void Init1(PROCESS_INFORMATION* pi)
{
	pi->dwProcessId = 12;
	pi->dwThreadId = 33;
}

void Init2(PROCESS_INFORMATION& pi)
{
	pi.dwProcessId = 33;
	pi.dwThreadId = 44;
}

int main()
{
	Process::ProcessInformation pi1, pi2;

	Init1(Helpers::GetAddressOf(pi1));
	Init2(Helpers::GetAddressOf(pi2));

	Process::Pipe p1;

	return 0;
}
