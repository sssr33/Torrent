#pragma once

#include <string>

namespace Terminal {
	class ITerminalParser
	{
	public:
		virtual ~ITerminalParser() = default;

		virtual void Process(const std::wstring_view string) = 0;
	};
}
