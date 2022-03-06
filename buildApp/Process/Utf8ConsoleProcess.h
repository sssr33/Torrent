#pragma once
#include "ProcessTask.h"
#include "TerminalParser/DefaultTerminalParser.h"

#include <memory>

namespace Process
{
    class Utf8ConsoleProcess
    {
    public:
        static void Run(const ProcessTaskParameters& params, std::shared_ptr<Terminal::ITerminalHandler> handler);

    private:
		class ProcessHandler final : public Process::IProcessTaskHandler
		{
		public:
			explicit ProcessHandler(std::shared_ptr<Terminal::ITerminalHandler> handler);

			void OnOutput(const void* data, size_t size, Process::IStdIn& stdIn) override;
			void OnError(const void* data, size_t size, Process::IStdIn& stdIn) override;

		private:
			std::unique_ptr<Terminal::ITerminalParser> terminalParser;
		};
    };
}
