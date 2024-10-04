#pragma once
#include "IWidget.h"
#include "WidgetAttachedPropertyAccessor.h"

#include <variant>

//class WidgetLayout {
//public:
//    struct SimpleAlignment {
//        WidgetHorizontalAlignment horizontalAlignment = WidgetHorizontalAlignment::Left;
//        WidgetVerticalAlignment verticalAlignment = WidgetVerticalAlignment::Top;
//    };
//
//    using Alignment = std::variant<SimpleAlignment, WidgetMarginAlignment>;
//
//    static Alignment GetWidgetAlignment(IWidget& widget);
//
//    static std::optional<WidgetVerticalAlignment> GetWidgetVerticalAlignment(IWidget& widget);
//    static void SetWidgetVerticalAlignment(IWidget& widget, WidgetVerticalAlignment alignment);
//
//    static std::optional<WidgetHorizontalAlignment> GetWidgetHorizontalAlignment(IWidget& widget);
//    static void SetWidgetHorizontalAlignment(IWidget& widget, WidgetHorizontalAlignment alignment);
//
//    static std::optional<WidgetMarginAlignment> GetWidgetMarginAlignment(IWidget& widget);
//    static void SetWidgetMarginAlignment(IWidget& widget, const WidgetMarginAlignment& alignment);
//
//private:
//    static const WidgetAttachedPropertyAccessor<WidgetVerticalAlignment> VerticalAlignmentPropAccessor;
//    static const WidgetAttachedPropertyAccessor<WidgetHorizontalAlignment> HorizontalAlignmentPropAccessor;
//    static const WidgetAttachedPropertyAccessor<WidgetMarginAlignment> MarginAlignmentPropAccessor;
//};
