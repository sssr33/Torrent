#pragma once
#include "Process/IStdIn.h"

#include <string>

namespace Terminal {
	enum class LineFeedMode {
		Unknown,
		NoReturn,
		WithReturn,
		DependsOnMode
	};

	class ITerminalHandler
	{
	public:
		virtual ~ITerminalHandler() = default;

		virtual void SetStdIn(Process::IStdIn* input) = 0;

		virtual void Print(const std::wstring_view string) = 0;
		virtual void LineFeed(LineFeedMode mode) = 0;
		virtual void CarriageReturn() = 0;
	};
}
