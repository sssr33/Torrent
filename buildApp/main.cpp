#include "BuildAppConfig.h"
#include "Helpers/Utf.h"
#include "Process/Utf8ConsoleProcess.h"

#include "PseudoConsoleTest.h"

class TestConsoleHandler : public Terminal::ITerminalHandler
{
public:
	std::wstring str;

	void Print(const std::wstring_view string) override
	{
        if (carriageReturn && !lineFeed) {
            while (!str.empty() && str.back() != '\r' && str.back() != '\n') {
                str.pop_back();
            }
        }

		str += string;

        carriageReturn = false;
        lineFeed = false;
	}

    void LineFeed(Terminal::LineFeedMode mode) override {
        str += L"\r\n";
        lineFeed = true;
    }

    void CarriageReturn() override {
        carriageReturn = true;
    }

private:
    bool carriageReturn = false;
    bool lineFeed = false;
};

void RunTest(std::wstring encoding);



int main()
{
    if(false)
	{
		PseudoConsoleTest::Test();
	}

	Helpers::Utf::Test();

	while (true)
	{
		RunTest(L"ascii");
		RunTest(L"u7");
		RunTest(L"u8");
		RunTest(L"u16");
		RunTest(L"---");
	}
	

	return 0;
}

void RunTest(std::wstring encoding)
{
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"ping google.com";

	/*procParams.exePath = LR"(C:\Users\sssr3\source\repos\ConsoleApp1Sharp\ConsoleApp1Sharp\bin\Release\netcoreapp3.1\ConsoleApp1Sharp.exe)";
	procParams.commandLine = L"ConsoleApp1Sharp.exe " + encoding;*/

	auto handler = std::make_shared<TestConsoleHandler>();

	Process::Utf8ConsoleProcess::Run(procParams, handler);

	return;
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
