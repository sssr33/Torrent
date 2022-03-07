#pragma once
#include "IPlatformFactory.h"

#include <memory>

namespace Platform {
    std::unique_ptr<IPlatformFactory> CreateWinPlatformFactory();
}
