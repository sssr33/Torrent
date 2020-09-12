#include "BuildAppConfig.h"
#include "Process/ProcessTask.h"

#include <iostream>

int main()
{
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"ping google.com";

	Process::ProcessTask::Run(procParams);

	return 0;
}
