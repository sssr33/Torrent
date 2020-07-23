#pragma once
#include "WinApiDeleters.h"

#include <memory>

namespace Helpers
{
	using UniqueCloseHandle = std::unique_ptr<Helpers::HandleVal, Helpers::HandleCloseDeleter>;
}
