#pragma once
#include "WinApiSmartHandle.h"
#include "Strong.h"

namespace Arg
{
	using ManualReset = Helpers::Strong<bool, struct ManualResetParameter>;
	using InitialSet = Helpers::Strong<bool, struct InitialSetParameter>;
}

namespace Helpers
{
	class WinApiEvent
	{
	public:
		WinApiEvent();
		WinApiEvent(Arg::ManualReset manualReset, Arg::InitialSet initialSet);

		HANDLE GetEventHandle() const;
		void ResetEvent() const;

	private:
		UniqueCloseHandle evt;
	};
}
