#pragma once
#include "ITerminalParser.h"
#include "ITerminalHandler.h"

#include <memory>

namespace Terminal {
    std::unique_ptr<ITerminalParser> CreateDefaultTerminalParser(std::shared_ptr<ITerminalHandler> handler);
}
