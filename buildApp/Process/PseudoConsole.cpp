#include "PseudoConsole.h"
#include "Helpers/WinApiException.h"
#include "Helpers/HResultException.h"
#include "Helpers/GetAddressOf.h"

namespace Process
{
    PseudoConsole::PseudoConsole(HANDLE read, HANDLE write)
		: console(MakePseudoConsole(read, write))
		, startupInfo(this->console.get())
    {}

	PseudoConsole::~PseudoConsole()
	{

	}

	STARTUPINFOEXW PseudoConsole::GetStartupInfoForConsole() const
	{
		return this->startupInfo.GetStartupInfoForConsole();
	}

	PseudoConsole::PseudoConsoleCloseHandle PseudoConsole::MakePseudoConsole(HANDLE read, HANDLE write)
	{
		PseudoConsoleCloseHandle handle;

		// Determine required size of Pseudo Console
		CONSOLE_SCREEN_BUFFER_INFO csbi{};
		HANDLE hConsole{ GetStdHandle(STD_OUTPUT_HANDLE) };

		if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		{
			throw Helpers::WinApiException();
		}

		HRESULT hr = S_OK;
		COORD consoleSize{};

		consoleSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		consoleSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

		// Create the Pseudo Console of the required size, attached to the PTY-end of the pipes
		hr = CreatePseudoConsole(consoleSize, read, write, 0, Helpers::GetAddressOf(handle));
		Helpers::ThrowIfFailed(hr);

		return handle;
	}

	void PseudoConsole::PseudoConsoleCloseDeleter::operator()(HPCONVal* handle)
	{
		ClosePseudoConsole(handle);
	}

	PseudoConsole::StartupInfo::StartupInfo(HPCON hPC)
	{
		if (!hPC || hPC == INVALID_HANDLE_VALUE)
		{
			throw Helpers::HResultException(E_UNEXPECTED);
		}

		SIZE_T attrListSize{};

		// Get the size of the thread attribute list.
		// This initial call will return an error by design. This is expected behavior.
		InitializeProcThreadAttributeList(NULL, 1, 0, &attrListSize);

		// Allocate a thread attribute list of the correct size
		this->attrList.resize(static_cast<size_t>(attrListSize));

		this->info.StartupInfo.cb = sizeof(STARTUPINFOEXW);
		this->info.lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(this->attrList.data());

		if (!this->info.lpAttributeList)
		{
			throw Helpers::HResultException(E_OUTOFMEMORY);
		}

		if (!InitializeProcThreadAttributeList(this->info.lpAttributeList, 1, 0, &attrListSize))
		{
			throw Helpers::WinApiException();
		}

		if (!UpdateProcThreadAttribute(
			this->info.lpAttributeList,
			0,
			PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
			hPC,
			sizeof(HPCON),
			NULL,
			NULL))
		{
			throw Helpers::WinApiException();
		}
	}

	PseudoConsole::StartupInfo::~StartupInfo()
	{
		if (this->attrList.empty())
		{
			// moved objects will have empty vector
			// so that we will know on which object call to DeleteProcThreadAttributeList is needed
			return;
		}

		DeleteProcThreadAttributeList(this->info.lpAttributeList);
	}

	STARTUPINFOEXW PseudoConsole::StartupInfo::GetStartupInfoForConsole() const
	{
		return this->info;
	}
}
