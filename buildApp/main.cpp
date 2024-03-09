#include "BuildAppConfig.h"
#include "Helpers/Utf.h"
#include "Process/Utf8ConsoleProcess.h"
#include "Platform/WinPlatformFactory.h"
#include "BuildAppConfig.h"

#include "PseudoConsoleTest.h"

#include <memory>
#include <algorithm>
#include <iterator>
#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <cassert>

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
	Release,

	Count
};

enum class BuildArch {
	Unknown,
	Win32,
	x64,

	Count
};

std::wstring ToDefaultString(BuildArch arch);
std::wstring ToDefaultString(BuildConfig config);

template<class FnT = std::function<void(BuildArch, BuildConfig)>>
void ForAllArchConfig(FnT fn) {
	for (auto arch = static_cast<std::underlying_type_t<BuildArch>>(BuildArch::Unknown) + 1;
		arch < static_cast<std::underlying_type_t<BuildArch>>(BuildArch::Count);
		++arch)
	{
		for (auto config = static_cast<std::underlying_type_t<BuildConfig>>(BuildConfig::Unknown) + 1;
			config < static_cast<std::underlying_type_t<BuildConfig>>(BuildConfig::Count);
			++config)
		{
			fn(static_cast<BuildArch>(arch), static_cast<BuildConfig>(config));
		}
	}
}

class ZLibBuild;

class LibBuild {
public:
	LibBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath);
	virtual ~LibBuild() = default;

	BuildArch GetBuildArch() const;
	BuildConfig GetBuildConfig() const;

	const std::wstring& GetSrcPath() const;
	const std::wstring& GetBuildBasePath() const;

protected:
	const ZLibBuild& FindZlibBuild(const std::vector<ZLibBuild>& zlibBuilds) const;

private:
	BuildArch arch;
	BuildConfig config;
	std::wstring srcPath;
	std::wstring buildBasePath;
};

class ZLibBuild : public LibBuild {
public:
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

	std::wstring GetBuildFolder() const;
	std::wstring GetBuildPath() const;
	std::wstring GetBaseCmdCommand() const;

	const ZLibBuild& zlibBuild;
	std::unique_ptr<Filesystem::IFilesystem> buildFolderFs;
};

class BoostBuild : public LibBuild {
public:
	BoostBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath,
		const std::vector<ZLibBuild>& zlibBuilds,
		std::unique_ptr<Filesystem::IFilesystem> buildFolderFs,
		std::shared_ptr<std::atomic<int>> inProgressBuildsCount);

	void InitJobCount(size_t jobCount);

	void DoAllSteps() const;

private:
	void Bootstrap() const;
	void Configure() const;
	void Build() const;
	void Install() const;
	void BootstrapCleanUp() const;

	std::wstring GetBuildFolder() const;
	std::wstring GetBuildPath() const;
	std::wstring GetBaseCmdCommand() const;
	std::wstring GetVsVarsCommand() const;
	std::wstring GetBoostBuildConfigName() const;
	std::wstring GetBoostAddressModelNumber() const;
	std::wstring GetBoostPrefixDir() const;
	std::wstring GetBoostBuildDir() const;
	std::wstring GetBoostStageDir() const;

	const ZLibBuild& zlibBuild;
	std::unique_ptr<Filesystem::IFilesystem> buildFolderFs;
	std::shared_ptr<std::atomic<int>> inProgressBuildsCount;
	size_t jobCount = 8;

	static std::once_flag bootstrapOnce;
};

std::once_flag BoostBuild::bootstrapOnce;

void RunTest(std::wstring encoding);
std::wstring ToWString(std::string_view str);
std::wstring Quote(const std::wstring& str);
void JoinAll(std::vector<std::thread>& tasks);

#define BUILD_CLEAN 0
#define BUILD_ZLIB 0
#define BUILD_OPENSSL 0
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
	auto boostSrcPath = solutionPath + L"boost";
	auto libtorrentSrcPath = solutionPath + L"libtorrent";
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

	ForAllArchConfig([&](BuildArch arch, BuildConfig config)
		{
			zlibBuilds.emplace_back(arch, config, zlibSrcPath, buildPath);
		});

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

	std::vector<OpenSSLBuild> openSslBuilds;

	ForAllArchConfig([&](BuildArch arch, BuildConfig config)
		{
			auto buildFolderFs = platformFactory->CreateFilesystem(buildPath);
			openSslBuilds.emplace_back(arch, config, openSslSrcPath, buildPath, zlibBuilds, std::move(buildFolderFs));
		});

#if BUILD_OPENSSL
	for (auto& build : openSslBuilds) {
		tasks.push_back(std::thread(&OpenSSLBuild::DoAllSteps, std::cref(build)));
	}

	JoinAll(tasks);
#endif

	std::cout << "OpenSSL build finished" << "\n\n";

	std::cout << "Boost build start" << "\n";

	std::vector<BoostBuild> boostBuilds;
	auto boostInProgressBuildsCount = std::make_shared<std::atomic<int>>();

	ForAllArchConfig([&](BuildArch arch, BuildConfig config)
		{
			auto buildFolderFs = platformFactory->CreateFilesystem(buildPath);
			boostBuilds.emplace_back(arch, config, boostSrcPath, buildPath, zlibBuilds, std::move(buildFolderFs), boostInProgressBuildsCount);
		});

	*boostInProgressBuildsCount = static_cast<int>(boostBuilds.size());

#if BUILD_BOOST
	// for now do not build all configs and architectures at once
	// it has some build errors
	// TODO check build errors in parallel build
	/*size_t boostJobCount = std::thread::hardware_concurrency() / boostBuilds.size();

	for (auto& build : boostBuilds) {
		build.InitJobCount(boostJobCount);
		tasks.push_back(std::thread(&BoostBuild::DoAllSteps, std::cref(build)));
	}

	JoinAll(tasks);*/

	// but build serially with max threads for each build
	size_t boostJobCount = std::thread::hardware_concurrency();

	for (auto& build : boostBuilds) {
		build.InitJobCount(boostJobCount);
		build.DoAllSteps();
	}
#endif

	std::cout << "Boost build finished" << "\n\n";

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

const ZLibBuild& LibBuild::FindZlibBuild(const std::vector<ZLibBuild>& zlibBuilds) const {
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
	this->buildFolderFs->CreateFolder(this->GetBuildFolder());

	const wchar_t* openSslArch = nullptr;
	const wchar_t* openSslConfig = nullptr;

	switch (this->GetBuildArch()) {
	case BuildArch::Win32:
		openSslArch = L"VC-WIN32";
		break;
	case BuildArch::x64:
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

	auto opensslDir = Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder() + L"/install/dir");
	auto opensslPrefix = Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder() + L"/install/prefix");
	auto zlibIncludePath = this->zlibBuild.GetIncludePath();
	auto zlibLibPath = this->zlibBuild.GetStaticLibPath();

	Process::ProcessTaskParameters procParams;

	procParams.commandLine =
		this->GetBaseCmdCommand()
		+ L" && "
		+ L"perl " + this->GetSrcPath() + L"/Configure" + L" " + openSslArch + L" " + openSslConfig + L" --openssldir=" + opensslDir + L" --prefix=" + opensslPrefix + L" --with-zlib-include=" + zlibIncludePath + L" --with-zlib-lib=" + zlibLibPath + L" zlib"
		;

	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void OpenSSLBuild::Build() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine =
		this->GetBaseCmdCommand()
		+ L" && "
		+ L"nmake"
		;

	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void OpenSSLBuild::Install() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine =
		this->GetBaseCmdCommand()
		+ L" && "
		+ L"nmake install"
		;

	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

std::wstring OpenSSLBuild::GetBuildFolder() const {
	return L"openssl_" + ToDefaultString(this->GetBuildArch()) + L'_' + ToDefaultString(this->GetBuildConfig());
}

std::wstring OpenSSLBuild::GetBuildPath() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder());
}

std::wstring OpenSSLBuild::GetBaseCmdCommand() const {
	const wchar_t* vcvarsallArch = nullptr;

	switch (this->GetBuildArch()) {
	case BuildArch::Win32:
		vcvarsallArch = L"x86";
		break;
	case BuildArch::x64:
		vcvarsallArch = L"x64";
		break;
	default:
		throw std::runtime_error("OpenSSLBuild::Configure: bad arch");
	}

	// commands:
	// 1. cd to build folder
	// 2. apply VC environment
	auto baseCommand =
		std::wstring()
		+ L"cmd.exe /C "
		+ L"cd /d " + this->GetBuildPath()
		+ L" && "
		+ Quote(LR"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat)") + L" " + vcvarsallArch
		;

	return baseCommand;
}

BoostBuild::BoostBuild(BuildArch arch, BuildConfig config, std::wstring srcPath, std::wstring buildBasePath,
	const std::vector<ZLibBuild>& zlibBuilds,
	std::unique_ptr<Filesystem::IFilesystem> buildFolderFs,
	std::shared_ptr<std::atomic<int>> inProgressBuildsCount)
	: LibBuild(arch, config, std::move(srcPath), std::move(buildBasePath))
	, zlibBuild(this->FindZlibBuild(zlibBuilds))
	, buildFolderFs(std::move(buildFolderFs))
	, inProgressBuildsCount(std::move(inProgressBuildsCount))
{}

void BoostBuild::InitJobCount(size_t jobCount) {
	this->jobCount = std::clamp(jobCount, (size_t)1, (size_t)std::thread::hardware_concurrency());
}

void BoostBuild::DoAllSteps() const {
	std::call_once(BoostBuild::bootstrapOnce, &BoostBuild::Bootstrap, this);

	this->Configure();
	this->Build();
	this->Install();

	if (this->inProgressBuildsCount) {
		int inProgressBuildCurrentCount = --(*this->inProgressBuildsCount);
		if (inProgressBuildCurrentCount == 0) {
			// do it once after all builds finished
			this->BootstrapCleanUp();
		}
	}
	else {
		// bootstrap cleanup skipped
		assert(false);
	}
}

void BoostBuild::Bootstrap() const {
	Process::ProcessTaskParameters procParams;

	procParams.commandLine =
		std::wstring()
		+ L"cmd.exe /C "
		+ L"cd /d " + this->GetSrcPath()
		+ L" && "
		+ this->GetVsVarsCommand()
		+ L" && "
		+ L"bootstrap.bat"
		;

	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void BoostBuild::Configure() const {
	this->buildFolderFs->CreateFolder(this->GetBuildFolder());
}

void BoostBuild::Build() const {
	auto buildCmd =
		std::wstring()
		+ L"b2"
		+ L" " + this->GetBoostBuildConfigName()
		+ L" address-model=" + this->GetBoostAddressModelNumber()
		+ L" threading=multi"
		+ L" --toolset=msvc"
		+ L" -j " + std::to_wstring(this->jobCount)
		+ L" link=static"
		+ L" runtime-link=shared"
		+ L" --prefix=" + this->GetBoostPrefixDir()
		+ L" --build-dir=" + this->GetBoostBuildDir()
		+ L" --stagedir=" + this->GetBoostStageDir()
		+ L" -sZLIB_INCLUDE=" + this->zlibBuild.GetIncludePath()
		+ L" -sZLIB_LIBPATH=" + this->zlibBuild.GetStaticLibPath()
		+ L" -sZLIB_SOURCE=" + this->zlibBuild.GetSrcPath()
		;

	Process::ProcessTaskParameters procParams;

	procParams.commandLine =
		std::wstring()
		+ L"cmd.exe /C "
		+ L"cd /d " + this->GetSrcPath()
		+ L" && "
		+ this->GetVsVarsCommand()
		+ L" && "
		+ buildCmd
		;

	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void BoostBuild::Install() const {
	auto installCmd =
		std::wstring()
		+ L"b2"
		+ L" " + this->GetBoostBuildConfigName()
		+ L" address-model=" + this->GetBoostAddressModelNumber()
		+ L" threading=multi"
		+ L" --toolset=msvc"
		+ L" -j " + std::to_wstring(this->jobCount)
		+ L" link=static"
		+ L" runtime-link=shared"
		+ L" --prefix=" + this->GetBoostPrefixDir()
		+ L" --build-dir=" + this->GetBoostBuildDir()
		+ L" --stagedir=" + this->GetBoostStageDir()
		+ L" -sZLIB_INCLUDE=" + this->zlibBuild.GetIncludePath()
		+ L" -sZLIB_LIBPATH=" + this->zlibBuild.GetStaticLibPath()
		+ L" -sZLIB_SOURCE=" + this->zlibBuild.GetSrcPath()
		+ L" install"
		;

	Process::ProcessTaskParameters procParams;

	procParams.commandLine =
		std::wstring()
		+ L"cmd.exe /C "
		+ L"cd /d " + this->GetSrcPath()
		+ L" && "
		+ this->GetVsVarsCommand()
		+ L" && "
		+ installCmd
		;

	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

void BoostBuild::BootstrapCleanUp() const {
	Process::ProcessTaskParameters procParams;

	// clean src folder from bootstrap files
	// build results are not in src folder but in this->GetBuildFolder()
	procParams.commandLine =
		std::wstring()
		+ L"cmd.exe /C "
		+ L"cd /d " + this->GetSrcPath()
		+ L" && "
		+ L"git clean -xfd"
		+ L" && "
		+ L"git submodule foreach --recursive git clean -xfd"
		;

	auto handler = std::make_shared<TestConsoleHandler>();
	Process::Utf8ConsoleProcess::Run(procParams, handler);
}

std::wstring BoostBuild::GetBuildFolder() const {
	return L"boost_" + ToDefaultString(this->GetBuildArch()) + L'_' + ToDefaultString(this->GetBuildConfig());
}

std::wstring BoostBuild::GetBuildPath() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder());
}

std::wstring BoostBuild::GetBaseCmdCommand() const {
	// commands:
	// 1. cd to build folder
	// 2. apply VC environment
	auto baseCommand =
		std::wstring()
		+ L"cmd.exe /C "
		+ L"cd /d " + this->GetBuildPath()
		+ L" && "
		+ this->GetVsVarsCommand();
		;

	return baseCommand;
}

std::wstring BoostBuild::GetVsVarsCommand() const {
	const wchar_t* vcvarsallArch = nullptr;

	switch (this->GetBuildArch()) {
	case BuildArch::Win32:
		vcvarsallArch = L"x86";
		break;
	case BuildArch::x64:
		vcvarsallArch = L"x64";
		break;
	default:
		throw std::runtime_error("BoostBuild::Configure: bad arch");
	}

	return Quote(LR"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat)") + L" " + vcvarsallArch;
}

std::wstring BoostBuild::GetBoostBuildConfigName() const {
	switch (this->GetBuildConfig()) {
	case BuildConfig::Debug:
		return L"debug";
	case BuildConfig::Release:
		return L"release";
	default:
		throw std::runtime_error("BoostBuild::Configure: bad ConfigName");
	}
}

std::wstring BoostBuild::GetBoostAddressModelNumber() const {
	switch (this->GetBuildArch()) {
	case BuildArch::Win32:
		return L"32";
	case BuildArch::x64:
		return L"64";
	default:
		throw std::runtime_error("BoostBuild::Configure: bad arch");
	}
}

std::wstring BoostBuild::GetBoostPrefixDir() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder() + L"/prefix");
}

std::wstring BoostBuild::GetBoostBuildDir() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder() + L"/build");
}

std::wstring BoostBuild::GetBoostStageDir() const {
	return Quote(std::wstring(this->GetBuildBasePath()) + L"/" + this->GetBuildFolder() + L"/stage");
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
