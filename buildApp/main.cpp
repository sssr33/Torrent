#include "BuildAppConfig.h"
#include "Process/ProcessTask.h"
#include "Helpers/Utf.h"

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

struct Result
{
	std::string outStr;
	std::wstring woutStr;
	std::u32string u32outStr;
};

Result RunTest(std::wstring encoding);

int main()
{
	Helpers::Utf::Test();

	//Process::ProcessTaskParameters procParams;

	////procParams.commandLine = L"ping google.com";

	//procParams.exePath = LR"(C:\Users\sssr3\source\repos\ConsoleApp1Sharp\ConsoleApp1Sharp\bin\Release\netcoreapp3.1\ConsoleApp1Sharp.exe)";
	//procParams.commandLine = L"ConsoleApp1Sharp.exe ascii";

	//TestProcessHandler handler;

	//Process::ProcessTask::Run(procParams, handler);

	//std::string outStr(handler.outputSink.begin(), handler.outputSink.end());
	//std::wstring woutStr(reinterpret_cast<const wchar_t*>(handler.outputSink.data()), reinterpret_cast<const wchar_t*>(handler.outputSink.data() + handler.outputSink.size()));

	auto ascii = RunTest(L"ascii");
	auto u7 = RunTest(L"u7");
	auto u8 = RunTest(L"u8");
	auto u16 = RunTest(L"u16");

	return 0;
}

Result RunTest(std::wstring encoding)
{
	Process::ProcessTaskParameters procParams;

	//procParams.commandLine = L"ping google.com";

	procParams.exePath = LR"(C:\Users\sssr3\source\repos\ConsoleApp1Sharp\ConsoleApp1Sharp\bin\Release\netcoreapp3.1\ConsoleApp1Sharp.exe)";
	procParams.commandLine = L"ConsoleApp1Sharp.exe " + encoding;

	TestProcessHandler handler;

	Process::ProcessTask::Run(procParams, handler);

	Result res;

	res.outStr = std::string(handler.outputSink.begin(), handler.outputSink.end());
	res.woutStr = std::wstring(reinterpret_cast<const wchar_t*>(handler.outputSink.data()), reinterpret_cast<const wchar_t*>(handler.outputSink.data() + handler.outputSink.size()));
	res.u32outStr = Helpers::Utf::Cvt<std::u32string>(res.outStr);

	return res;
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


