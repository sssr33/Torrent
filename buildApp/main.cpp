#include "BuildAppConfig.h"
#include "Process/ProcessTask.h"

#include <iostream>

class TestProcessHandler final : public Process::IProcessTaskHandler
{
public:
	void OnOutput(const void* data, size_t size, Process::IStdIn& stdIn) override
	{

	}

	void OnError(const void* data, size_t size, Process::IStdIn& stdIn) override
	{

	}
};

int main()
{
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"ping google.com";

	TestProcessHandler handler;

	Process::ProcessTask::Run(procParams, handler);

	return 0;
}
