#include "RootWidget.h"

void RootWidget::Draw(WidgetRenderContext& ctx) {
    RootWidgetBase::DrawChildren(ctx);
}

WidgetMargin RootWidget::GetMargin() {
    return {};
}

void RootWidget::SetMargin(const WidgetMargin& margin) {}

DirectX::XMFLOAT2 RootWidget::GetSize() {
    return this->size;
}

void RootWidget::SetSize(const DirectX::XMFLOAT2& size) {
    this->size = size;
}

WidgetVerticalAlignment RootWidget::GetVerticalAlignment() {
    return WidgetVerticalAlignment::Top;
}

void RootWidget::SetVerticalAlignment(const WidgetVerticalAlignment& vertAlign) {}

WidgetHorizontalAlignment RootWidget::GetHorizontalAlignment() {
    return WidgetHorizontalAlignment::Left;
}

void RootWidget::SetHorizontalAlignment(const WidgetHorizontalAlignment& horzAlign) {}

std::optional<D2D1_RECT_F> RootWidget::GetActualRect() {
    return D2D1::RectF(0.f, 0.f, this->size.x, this->size.y);
}

WidgetAttachedPropertyValue RootWidget::GetAttachedProperty(const std::string& name) {
    return WidgetAttachedPropertyEmptyValue{};
}

void RootWidget::SetAttachedProperty(const std::string& name, WidgetAttachedPropertyValue value) {}

std::shared_ptr<IWidget> RootWidget::GetParent() {
    return nullptr;
}

void RootWidget::SetParent(std::weak_ptr<IWidget> parent) {}

std::optional<D2D1_RECT_F> RootWidget::GetChildLayoutRect(IWidget* child) {
    return this->GetActualRect();
}
