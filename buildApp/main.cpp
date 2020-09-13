#include "BuildAppConfig.h"
#include "Process/ProcessTask.h"

#include <iostream>
#include <vector>
#include <string>

class TestProcessHandler final : public Process::IProcessTaskHandler
{
public:
	std::vector<uint8_t> outputSink;
	std::vector<uint8_t> errorSink;

	void OnOutput(const void* data, size_t size, Process::IStdIn& stdIn) override
	{
		const uint8_t* bytes = static_cast<const uint8_t*>(data);

		outputSink.insert(outputSink.end(), bytes, bytes + size);
	}

	void OnError(const void* data, size_t size, Process::IStdIn& stdIn) override
	{
		const uint8_t* bytes = static_cast<const uint8_t*>(data);

		errorSink.insert(outputSink.end(), bytes, bytes + size);
	}
};

int main()
{
	Process::ProcessTaskParameters procParams;

	//procParams.commandLine = L"ping google.com";

	procParams.exePath = LR"(C:\Users\sssr3\source\repos\ConsoleApp1Sharp\ConsoleApp1Sharp\bin\Release\netcoreapp3.1\ConsoleApp1Sharp.exe)";
	procParams.commandLine = L"ConsoleApp1Sharp.exe u16";

	TestProcessHandler handler;

	Process::ProcessTask::Run(procParams, handler);

	std::string outStr(handler.outputSink.begin(), handler.outputSink.end());
	std::wstring woutStr(reinterpret_cast<const wchar_t*>(handler.outputSink.data()), reinterpret_cast<const wchar_t*>(handler.outputSink.data() + handler.outputSink.size()));

	return 0;
}

// https://devblogs.microsoft.com/commandline/windows-command-line-introducing-the-windows-pseudo-console-conpty/
// ConsoleApp1Sharp.exe
//using System;
//
//namespace ConsoleApp1Sharp
//{
//    class Program
//    {
//        static void Main(string[] args)
//        {
//            string mode = "default";
//
//            Console.OutputEncoding = System.Text.Encoding.Default;
//
//            if (args.Length > 0)
//            {
//                switch (args[0])
//                {
//                case "ascii":
//                    mode = "ascii";
//                    Console.OutputEncoding = System.Text.Encoding.ASCII;
//                    break;
//                case "u7":
//                    mode = "u7";
//                    Console.OutputEncoding = System.Text.Encoding.UTF7;
//                    break;
//                case "u8":
//                    mode = "u8";
//                    Console.OutputEncoding = System.Text.Encoding.UTF8;
//                    break;
//                case "u16":
//                    mode = "u16";
//                    Console.OutputEncoding = System.Text.Encoding.Unicode;
//                    break;
//                }
//            }
//
//            Console.WriteLine("code page: " + Console.OutputEncoding.CodePage);
//            Console.WriteLine(args.Length);
//            Console.WriteLine(mode);
//            Console.WriteLine("Hello World!");
//            Console.WriteLine("Привет мир!");
//            Console.WriteLine("你好，世界!");
//        }
//    }
//}


