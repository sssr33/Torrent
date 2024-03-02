﻿#include "BuildAppConfig.h"
#include "Helpers/Utf.h"
#include "Process/Utf8ConsoleProcess.h"
#include "Platform/WinPlatformFactory.h"
#include "BuildAppConfig.h"

#include "PseudoConsoleTest.h"

#include <algorithm>
#include <iterator>
#include <thread>
#include <stdexcept>
#include <iostream>

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

enum class BuildConfig {
	Unknown,
	Debug,
	Release
};

enum class BuildArch {
	Unknown,
	Win32,
	x64
};

std::wstring ToDefaultString(BuildArch arch);
std::wstring ToDefaultString(BuildConfig config);

class LibBuild {
public:
	LibBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath);
	virtual ~LibBuild() = default;

	BuildArch GetBuildArch() const;
	BuildConfig GetBuildConfig() const;

	const std::wstring& GetSrcPath() const;
	const std::wstring& GetBuildBasePath() const;

private:
	BuildArch arch;
	BuildConfig config;
	std::wstring srcPath;
	std::wstring buildBasePath;
};

struct ZLibBuild : public LibBuild {
	ZLibBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath);

	void DoAllSteps() const;

	std::wstring GetStaticLibPath() const;
	std::wstring GetIncludePath() const;

private:
	void Generate() const;
	void Build() const;
	void Install() const;

	std::wstring GetBuildPath() const;
	std::wstring GetInstallPath() const;

	std::wstring GetArchPathStr() const;
	std::wstring GetSolutionConfigStr() const;
};

class OpenSSLBuild : public LibBuild {
public:
	OpenSSLBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath,
		const std::vector<ZLibBuild>& zlibBuilds,
		std::unique_ptr<Filesystem::IFilesystem> buildFolderFs);

	void DoAllSteps() const;

private:
	void Configure() const;
	void Build() const;
	void Install() const;

	const ZLibBuild& FindZlibBuild(const std::vector<ZLibBuild>& zlibBuilds) const;

	std::wstring GetBuildFolder() const;
	std::wstring GetBuildPath() const;

	std::wstring openSslSrcPath;
	std::wstring buildPathBase;
	BuildArch arch;
	BuildConfig config;
	const ZLibBuild& zlibBuild;
	std::unique_ptr<Filesystem::IFilesystem> buildFolderFs;
};

void RunTest(std::wstring encoding);
std::wstring ToWString(std::string_view str);
std::wstring Quote(const std::wstring& str);
void JoinAll(std::vector<std::thread>& tasks);

#define BUILD_CLEAN 0
#define BUILD_ZLIB 0
#define BUILD_OPENSSL 1
#define BUILD_BOOST 1
#define BUILD_LIBTORRENT 1

int main()
{
	//RunTest(L"ascii");

	std::vector<std::thread> tasks;

	const std::wstring buildLibFolder = L"!!BUILD_LIB";

	auto solutionPath = ToWString(SOLUTION_FOLDER);
	auto buildPath = solutionPath + buildLibFolder;
	auto zlibSrcPath = solutionPath + L"zlib";
	auto openSslSrcPath = solutionPath + L"openssl";
	auto platformFactory = Platform::CreateWinPlatformFactory();

#if BUILD_CLEAN
	{
		TestProgress testProgress;
		auto fileSystem = platformFactory->CreateFilesystem(solutionPath);
		fileSystem->DeleteFolder(buildLibFolder, &testProgress);
	}
#endif

	std::cout << "Zlib build start" << "\n";

	auto zlibFileSystem = platformFactory->CreateFilesystem(zlibSrcPath);

	zlibFileSystem->CopyFile(L"zconfTmp.h", L"zconf.h");
	zlibFileSystem->DeleteFile(L"zconf.h");

	std::vector<ZLibBuild> zlibBuilds;

	zlibBuilds.emplace_back(BuildArch::x64, BuildConfig::Release, zlibSrcPath, buildPath);
	zlibBuilds.emplace_back(BuildArch::x64, BuildConfig::Debug, zlibSrcPath, buildPath);

	zlibBuilds.emplace_back(BuildArch::Win32, BuildConfig::Release, zlibSrcPath, buildPath);
	zlibBuilds.emplace_back(BuildArch::Win32, BuildConfig::Debug, zlibSrcPath, buildPath);

#if BUILD_ZLIB
	for (auto& build : zlibBuilds) {
		tasks.push_back(std::thread(&ZLibBuild::DoAllSteps, std::cref(build)));
	}

	JoinAll(tasks);
#endif

	zlibFileSystem->CopyFile(L"zconf.h", L"zconfTmp.h");
	zlibFileSystem->DeleteFile(L"zconfTmp.h");
	zlibFileSystem->DeleteFile(L"zconf.h.included");

	std::cout << "Zlib build finished" << "\n\n";

	std::cout << "OpenSSL build start" << "\n";

	auto buildFolderFs = platformFactory->CreateFilesystem(buildPath);

	auto openSslx64Rel = OpenSSLBuild(BuildArch::x64, BuildConfig::Release, openSslSrcPath, buildPath, zlibBuilds, std::move(buildFolderFs));
	openSslx64Rel.DoAllSteps();

	std::cout << "OpenSSL build finished" << "\n\n";

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

std::wstring ToDefaultString(BuildArch arch) {
	switch (arch) {
	case BuildArch::Win32:
		return L"Win32";
	case BuildArch::x64:
		return L"x64";
	}

	return {};
}

std::wstring ToDefaultString(BuildConfig config) {
	switch (config) {
	case BuildConfig::Debug:
		return L"Debug";
	case BuildConfig::Release:
		return L"Release";
	}

	return {};
}

LibBuild::LibBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath)
	: arch(arch)
	, config(config)
	, srcPath(std::move(srcPath))
	, buildBasePath(std::move(buildBasePath))
{}

BuildArch LibBuild::GetBuildArch() const {
	return this->arch;
}

BuildConfig LibBuild::GetBuildConfig() const {
	return this->config;
}

const std::wstring& LibBuild::GetSrcPath() const {
	return this->srcPath;
}

const std::wstring& LibBuild::GetBuildBasePath() const {
	return this->buildBasePath;
}

ZLibBuild::ZLibBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath)
	: LibBuild(arch, config, std::move(srcPath), std::move(buildBasePath))
{}

void ZLibBuild::DoAllSteps() const {
	this->Generate();
	this->Build();
	this->Install();
}

std::wstring ZLibBuild::GetStaticLibPath() const {
	const wchar_t* libName = nullptr;

	switch (this->GetBuildConfig()) {
	case BuildConfig::Debug:
		libName = L"zlibstaticd.lib";
		break;
	case BuildConfig::Release:
		libName = L"zlibstatic.lib";
		break;
	default:
		throw std::runtime_error("GetStaticLibPath: unknown config");
	}

	auto res = this->GetInstallPath() + L"/lib/" + libName;
	return res;
}

std::wstring ZLibBuild::GetIncludePath() const {
	auto res = this->GetInstallPath() + L"/include";
	return res;
}

void ZLibBuild::Generate() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"cmake -G \"Visual Studio 16 2019\" -A " + this->GetArchPathStr() + L" -S " + Quote(this->GetSrcPath()) + L" -B " + this->GetBuildPath() + L" --install-prefix " + this->GetInstallPath();

	auto handler = std::make_shared<TestConsoleHandler>();

	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void ZLibBuild::Build() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"cmake --build " + this->GetBuildPath() + L" --target \"zlib\" " + L" --config " + this->GetSolutionConfigStr();
	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);

	procParams.commandLine = L"cmake --build " + this->GetBuildPath() + L" --target \"zlibstatic\" " + L" --config " + this->GetSolutionConfigStr();
	handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void ZLibBuild::Install() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine = L"cmake --install " + this->GetBuildPath() + L" --config " + this->GetSolutionConfigStr();
	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

std::wstring ZLibBuild::GetBuildPath() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/zlib_" + this->GetArchPathStr() + L'_' + this->GetSolutionConfigStr());
}

std::wstring ZLibBuild::GetInstallPath() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/zlib_" + this->GetArchPathStr() + L'_' + this->GetSolutionConfigStr()) + L"/install";
}

std::wstring ZLibBuild::GetArchPathStr() const {
	return ToDefaultString(this->GetBuildArch());
}

std::wstring ZLibBuild::GetSolutionConfigStr() const {
	return ToDefaultString(this->GetBuildConfig());
}

OpenSSLBuild::OpenSSLBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath,
	const std::vector<ZLibBuild>& zlibBuilds,
	std::unique_ptr<Filesystem::IFilesystem> buildFolderFs)
	: LibBuild(arch, config, std::move(srcPath), std::move(buildBasePath))
	, zlibBuild(this->FindZlibBuild(zlibBuilds))
	, buildFolderFs(std::move(buildFolderFs))
{}

void OpenSSLBuild::DoAllSteps() const {
	this->Configure();
	this->Build();
	this->Install();
}

void OpenSSLBuild::Configure() const {
	Process::ProcessTaskParameters procParams;

	/*
	* perl C:\work_Chaikovsky\!myReps\Torrent\openssl\Configure VC-WIN32 --release --openssldir=C:\work_Chaikovsky\!myReps\Torrent\openssl\build_inst\dir --prefix=C:\work_Chaikovsky\!myReps\Torrent\openssl\build_inst\prefix --with-zlib-include=C:\work_Chaikovsky\!myReps\Torrent\!!BUILD_LIB\zlib_Win32_Release\install\include --with-zlib-lib=C:\work_Chaikovsky\!myReps\Torrent\!!BUILD_LIB\zlib_Win32_Release\install\lib\zlibstatic.lib zlib
	*/

	this->buildFolderFs->CreateFolder(this->GetBuildFolder());

	const wchar_t* vcvarsallArch = nullptr;
	const wchar_t* openSslArch = nullptr;
	const wchar_t* openSslConfig = nullptr;

	switch (this->GetBuildArch()) {
	case BuildArch::Win32:
		vcvarsallArch = L"x86";
		openSslArch = L"VC-WIN32";
		break;
	case BuildArch::x64:
		vcvarsallArch = L"x64";
		openSslArch = L"VC-WIN64A";
		break;
	default:
		throw std::runtime_error("OpenSSLBuild::Configure: bad arch");
	}

	switch (this->GetBuildConfig()) {
	case BuildConfig::Debug:
		openSslConfig = L"--debug";
		break;
	case BuildConfig::Release:
		openSslConfig = L"--release";
		break;
	default:
		throw std::runtime_error("OpenSSLBuild::Configure: bad config");
	}

	auto opensslDir = Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder() + L"/build_inst/dir");
	auto opensslPrefix = Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder() + L"/build_inst/prefix");
	auto zlibIncludePath = this->zlibBuild.GetIncludePath();
	auto zlibLibPath = this->zlibBuild.GetStaticLibPath();

	/*procParams.commandLine =
		std::wstring()
		+ L"cmd.exe /C \"echo %PATH% && nasm.exe\""
		;*/

	procParams.commandLine =
		std::wstring()
		+ L"cmd.exe /C "
		+ L"cd /d " + this->GetBuildPath()
		+ L" && "
		+ Quote(LR"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat)") + L" x64"
		+ L" && "
		+ L"perl " + this->GetSrcPath() + L"/Configure" + L" " + openSslArch + L" " + openSslConfig + L" --openssldir=" + opensslDir + L" --prefix=" + opensslPrefix + L" --with-zlib-include=" + zlibIncludePath + L" --with-zlib-lib=" + zlibLibPath + L" zlib"
		;

	auto handler = std::make_shared<TestConsoleHandler>();

	Process::Utf8ConsoleProcess::Run(procParams, handler);

	int stop = 234;
}

void OpenSSLBuild::Build() const {

}

void OpenSSLBuild::Install() const {

}

const ZLibBuild& OpenSSLBuild::FindZlibBuild(const std::vector<ZLibBuild>& zlibBuilds) const {
	auto it = std::find_if(std::begin(zlibBuilds), std::end(zlibBuilds), [this](const ZLibBuild& build)
		{
			bool found = this->GetBuildArch() == build.GetBuildArch()
				&& this->GetBuildConfig() == build.GetBuildConfig();

			return found;
		});

	if (it == std::end(zlibBuilds)) {
		throw std::runtime_error("OpenSSLBuild: ZLibBuild config/arch not found");
	}

	return *it;
}

std::wstring OpenSSLBuild::GetBuildFolder() const {
	return L"openssl_" + ToDefaultString(this->GetBuildArch()) + L'_' + ToDefaultString(this->GetBuildConfig());
}

std::wstring OpenSSLBuild::GetBuildPath() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder());
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
