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

			void SetStdIn(IStdIn* input) override;

			void OnOutput(const void* data, size_t size) override;
			void OnError(const void* data, size_t size) override;

		private:
			std::unique_ptr<Terminal::ITerminalParser> terminalParser;
			std::shared_ptr<Terminal::ITerminalHandler> handler;
		};
    };
}
