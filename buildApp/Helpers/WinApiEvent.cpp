#include "WinApiEvent.h"
#include "WinApiException.h"

namespace Helpers
{
	WinApiEvent::WinApiEvent()
		: WinApiEvent(Arg::ManualReset(false), Arg::InitialSet(false))
	{}

	WinApiEvent::WinApiEvent(Arg::ManualReset manualReset, Arg::InitialSet initialSet)
	{
		DWORD evtFlags = 0;

		evtFlags |= manualReset ? CREATE_EVENT_MANUAL_RESET : 0;
		evtFlags |= initialSet ? CREATE_EVENT_INITIAL_SET : 0;

		HANDLE handle = CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		if (!handle)
		{
			throw WinApiException();
		}

		this->evt.reset(handle);
	}

	HANDLE WinApiEvent::GetEventHandle() const
	{
		return this->evt.get();
	}

	void WinApiEvent::ResetEvent() const
	{
		if (!::ResetEvent(this->evt.get()))
		{
			throw WinApiException();
		}
	}
}
