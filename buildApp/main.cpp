#include "BuildAppConfig.h"
#include "Process/ProcessTask.h"

#include <iostream>

int main()
{
	// ping google.com
	Process::ProcessTask task(L"ping.exe");

	task.Run();

	return 0;
}
