#include "WidgetLayout.h"

//WidgetLayout::Alignment WidgetLayout::GetWidgetAlignment(IWidget& widget) {
//    if (auto margin = WidgetLayout::GetWidgetMarginAlignment(widget)) {
//        return *margin;
//    }
//
//    SimpleAlignment alignment;
//
//    if (auto valign = WidgetLayout::GetWidgetVerticalAlignment(widget)) {
//        alignment.verticalAlignment = *valign;
//    }
//    if (auto halign = WidgetLayout::GetWidgetHorizontalAlignment(widget)) {
//        alignment.horizontalAlignment = *halign;
//    }
//
//    return alignment;
//}
//
//std::optional<WidgetVerticalAlignment> WidgetLayout::GetWidgetVerticalAlignment(IWidget& widget) {
//    return WidgetLayout::VerticalAlignmentPropAccessor.Get(widget);
//}
//
//void WidgetLayout::SetWidgetVerticalAlignment(IWidget& widget, WidgetVerticalAlignment alignment) {
//    WidgetLayout::VerticalAlignmentPropAccessor.Set(widget, alignment);
//}
//
//std::optional<WidgetHorizontalAlignment> WidgetLayout::GetWidgetHorizontalAlignment(IWidget& widget) {
//    return WidgetLayout::HorizontalAlignmentPropAccessor.Get(widget);
//}
//
//void WidgetLayout::SetWidgetHorizontalAlignment(IWidget& widget, WidgetHorizontalAlignment alignment) {
//    WidgetLayout::HorizontalAlignmentPropAccessor.Set(widget, alignment);
//}
//
//std::optional<WidgetMarginAlignment> WidgetLayout::GetWidgetMarginAlignment(IWidget& widget) {
//    return WidgetLayout::MarginAlignmentPropAccessor.Get(widget);
//}
//
//void WidgetLayout::SetWidgetMarginAlignment(IWidget& widget, const WidgetMarginAlignment& alignment) {
//    WidgetLayout::MarginAlignmentPropAccessor.Set(widget, alignment);
//}
//
//const WidgetAttachedPropertyAccessor<WidgetVerticalAlignment> WidgetLayout::VerticalAlignmentPropAccessor("Layout.VerticalAlignment");
//const WidgetAttachedPropertyAccessor<WidgetHorizontalAlignment> WidgetLayout::HorizontalAlignmentPropAccessor("Layout.HorizontalAlignment");
//const WidgetAttachedPropertyAccessor<WidgetMarginAlignment> WidgetLayout::MarginAlignmentPropAccessor("Layout.MarginAlignment");
