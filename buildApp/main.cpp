#include "BuildAppConfig.h"
#include "Helpers/Utf.h"
#include "Process/Utf8ConsoleProcess.h"
#include "Platform/WinPlatformFactory.h"
#include "BuildAppConfig.h"

#include "PseudoConsoleTest.h"

#include <algorithm>
#include <iterator>
#include <thread>

class TestConsoleHandler : public Terminal::ITerminalHandler
{
public:
	std::wstring str;

	void SetStdIn(Process::IStdIn* input) override {
		this->input = input;
	}

	void Print(const std::wstring_view string) override
	{
        if (carriageReturn && !lineFeed) {
            while (!str.empty() && str.back() != '\r' && str.back() != '\n') {
                str.pop_back();
            }
        }

		str += string;

		if (!done && str.find(L"0x213") != std::wstring::npos) {
			done = true;

			std::string str = "echo 00xx\r\n";

			this->input->Write(str.data(), str.size());
		}

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
	bool done = false;
	Process::IStdIn* input = nullptr;
    bool carriageReturn = false;
    bool lineFeed = false;
};

class TestProgress : public Filesystem::IFilesystemProgress {
public:
	void ReportProgress(float progress) override {
		int stop = 234;
		stop++;
	}
};

struct ZLibBuildConfig {
	ZLibBuildConfig(std::wstring srcPath, std::wstring buildPathBase, std::wstring arch, std::wstring solutionConfig);

	void DoAllSteps() const;

private:
	void Generate() const;
	void Build() const;
	void Install() const;

	std::wstring GetBuildPath() const;
	std::wstring GetInstallPath() const;

	std::wstring srcPath;
	std::wstring buildPathBase;
	std::wstring arch;
	std::wstring solutionConfig;
};

void RunTest(std::wstring encoding);
std::wstring ToWString(std::string_view str);
std::wstring Quote(const std::wstring& str);
void JoinAll(std::vector<std::thread>& tasks);

int main()
{
	//RunTest(L"ascii");

	std::vector<std::thread> tasks;

	const std::wstring buildLibFolder = L"!!BUILD_LIB";

	auto solutionPath = ToWString(SOLUTION_FOLDER);
	auto zlibPath = solutionPath + L"zlib";
	auto zlibBuildPath = solutionPath + buildLibFolder;
	auto platformFactory = Platform::CreateWinPlatformFactory();

	{
		TestProgress testProgress;
		auto fileSystem = platformFactory->CreateFilesystem(solutionPath);
		fileSystem->DeleteFolder(buildLibFolder, &testProgress);
	}

	auto fileSystem = platformFactory->CreateFilesystem(zlibPath);

	fileSystem->CopyFile(L"zconfTmp.h", L"zconf.h");
	fileSystem->DeleteFile(L"zconf.h");

	auto zlibx64Rel = ZLibBuildConfig(zlibPath, zlibBuildPath, L"x64", L"Release");
	auto zlibx64Dbg = ZLibBuildConfig(zlibPath, zlibBuildPath, L"x64", L"Debug");

	auto zlibWin32Rel = ZLibBuildConfig(zlibPath, zlibBuildPath, L"Win32", L"Release");
	auto zlibWin32Dbg = ZLibBuildConfig(zlibPath, zlibBuildPath, L"Win32", L"Debug");

	tasks.push_back(std::thread(&ZLibBuildConfig::DoAllSteps, std::cref(zlibx64Rel)));
	tasks.push_back(std::thread(&ZLibBuildConfig::DoAllSteps, std::cref(zlibx64Dbg)));
	tasks.push_back(std::thread(&ZLibBuildConfig::DoAllSteps, std::cref(zlibWin32Rel)));
	tasks.push_back(std::thread(&ZLibBuildConfig::DoAllSteps, std::cref(zlibWin32Dbg)));

	JoinAll(tasks);

	fileSystem->CopyFile(L"zconf.h", L"zconfTmp.h");
	fileSystem->DeleteFile(L"zconfTmp.h");
	fileSystem->DeleteFile(L"zconf.h.included");

    /*if(false)
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
	}*/
	

	return 0;
}

void RunTest(std::wstring encoding)
{
	Process::ProcessTaskParameters procParams;

	//procParams.commandLine = L"cmd /k \"ping google.com & ping 3dnews.com & exit\"";

	procParams.commandLine = L"cmd /k \"echo 0x213\"";

	/*procParams.exePath = LR"(C:\Users\sssr3\source\repos\ConsoleApp1Sharp\ConsoleApp1Sharp\bin\Release\netcoreapp3.1\ConsoleApp1Sharp.exe)";
	procParams.commandLine = L"ConsoleApp1Sharp.exe " + encoding;*/

	auto handler = std::make_shared<TestConsoleHandler>();

	Process::Utf8ConsoleProcess::Run(procParams, handler);

	return;
}

std::wstring ToWString(std::string_view str) {
	std::wstring wstr;

	wstr.reserve(str.size());

	std::transform(str.begin(), str.end(), std::back_inserter(wstr), [](char ch) { return static_cast<wchar_t>(ch); });

	return wstr;
}

std::wstring Quote(const std::wstring& str) {
	return L'\"' + str + L'\"';
}

void JoinAll(std::vector<std::thread>& tasks) {
	for (auto& i : tasks) {
		i.join();
	}

	tasks.clear();
}

ZLibBuildConfig::ZLibBuildConfig(std::wstring srcPath, std::wstring buildPathBase, std::wstring arch, std::wstring solutionConfig)
	: srcPath(std::move(srcPath))
	, buildPathBase(std::move(buildPathBase))
	, arch(std::move(arch))
	, solutionConfig(std::move(solutionConfig))
{}

void ZLibBuildConfig::DoAllSteps() const {
	this->Generate();
	this->Build();
	this->Install();
}

void ZLibBuildConfig::Generate() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"cmake -G \"Visual Studio 16 2019\" -A " + std::wstring(arch) + L" -S " + Quote(this->srcPath) + L" -B " + this->GetBuildPath() + L" --install-prefix " + this->GetInstallPath();

	auto handler = std::make_shared<TestConsoleHandler>();

	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void ZLibBuildConfig::Build() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"cmake --build " + this->GetBuildPath() + L" --target \"zlib\" " + L" --config " + this->solutionConfig;
	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);

	procParams.commandLine = L"cmake --build " + this->GetBuildPath() + L" --target \"zlibstatic\" " + L" --config " + this->solutionConfig;
	handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void ZLibBuildConfig::Install() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"cmake --install " + this->GetBuildPath() + L" --config " + this->solutionConfig;
	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

std::wstring ZLibBuildConfig::GetBuildPath() const {
	return Quote(std::wstring(this->buildPathBase) + L"/zlib_" + std::wstring(arch) + L'_' + this->solutionConfig);
}

std::wstring ZLibBuildConfig::GetInstallPath() const {
	return Quote(std::wstring(this->buildPathBase) + L"/zlib_" + std::wstring(arch) + L'_' + this->solutionConfig) + L"/install";
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
