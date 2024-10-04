#pragma once
#include "WidgetVerticalAlignment.h"
#include "WidgetHorizontalAlignment.h"

#include <variant>
#include <cstdint>

using WidgetAttachedPropertyEmptyValue = std::monostate;

using WidgetAttachedPropertyValue = std::variant<
    WidgetAttachedPropertyEmptyValue,
    uint32_t
>;
