#pragma once

enum class WidgetMarginValueType {
    Pixel,
    Normalized
};

struct WidgetMarginValue {
    float value = 0.f;
    WidgetMarginValueType type = WidgetMarginValueType::Pixel;
};

struct WidgetMargin {
    WidgetMarginValue left;
    WidgetMarginValue top;
    WidgetMarginValue right;
    WidgetMarginValue bottom;
};
