#include "WidgetBase.h"

WidgetMargin WidgetBase::GetMargin() {
    return this->margin;
}

void WidgetBase::SetMargin(const WidgetMargin& margin) {
    this->margin = margin;
}

DirectX::XMFLOAT2 WidgetBase::GetSize() {
    return this->size;
}

void WidgetBase::SetSize(const DirectX::XMFLOAT2& size) {
    this->size = size;
}

WidgetVerticalAlignment WidgetBase::GetVerticalAlignment() {
    return this->vertAlign;
}

void WidgetBase::SetVerticalAlignment(const WidgetVerticalAlignment& vertAlign) {
    this->vertAlign = vertAlign;
}

WidgetHorizontalAlignment WidgetBase::GetHorizontalAlignment() {
    return this->horzAlign;
}

void WidgetBase::SetHorizontalAlignment(const WidgetHorizontalAlignment& horzAlign) {
    this->horzAlign = horzAlign;
}

std::optional<D2D1_RECT_F> WidgetBase::GetActualRect() {
    auto parent = this->GetParent();
    if (!parent) {
        return {};
    }

    auto layoutRect = parent->GetChildLayoutRect(this);
    if (!layoutRect) {
        return {};
    }

    auto pixelMargin = WidgetBase::MarginToPixelValue(
        this->GetMargin(),
        {
            layoutRect->right - layoutRect->left,
            layoutRect->bottom - layoutRect->top
        }
    );

    auto actualLayoutRect = *layoutRect;

    actualLayoutRect.left += pixelMargin.left.value;
    actualLayoutRect.top += pixelMargin.top.value;
    actualLayoutRect.right -= pixelMargin.right.value;
    actualLayoutRect.bottom -= pixelMargin.bottom.value;

    D2D1_RECT_F actualRect = this->VerticalAlign(actualLayoutRect);
    actualRect = this->HorizontalAlign(actualLayoutRect, actualRect);

    return actualRect;
}

WidgetAttachedPropertyValue WidgetBase::GetAttachedProperty(const std::string& name) {
    auto it = this->attachedProps.find(name);
    if (it != std::end(this->attachedProps)) {
        return it->second;
    }

    return WidgetAttachedPropertyEmptyValue{};
}

void WidgetBase::SetAttachedProperty(const std::string& name, WidgetAttachedPropertyValue value) {
    this->attachedProps[name] = std::move(value);
}

std::shared_ptr<IWidget> WidgetBase::GetParent() {
    return this->parent.lock();
}

void WidgetBase::SetParent(std::weak_ptr<IWidget> parent) {
    this->parent = parent;
}

std::optional<D2D1_RECT_F> WidgetBase::GetChildLayoutRect(IWidget* child) {
    return this->GetActualRect();
}

D2D1_RECT_F WidgetBase::VerticalAlign(const D2D1_RECT_F& layoutRect, D2D1_RECT_F rectToAlign /*= {}*/) const {
    switch (this->vertAlign) {
    case WidgetVerticalAlignment::Top:
        rectToAlign.top = layoutRect.top;
        rectToAlign.bottom = (std::min)(layoutRect.top + this->size.y, layoutRect.bottom);
        break;
    case WidgetVerticalAlignment::Center: {
        float halfHeight = this->size.y / 2.f;
        float centerY = (layoutRect.top + layoutRect.bottom) / 2.f;

        rectToAlign.top = (std::max)(centerY - halfHeight, layoutRect.top);
        rectToAlign.bottom = (std::min)(centerY + halfHeight, layoutRect.bottom);

        break;
    }
    case WidgetVerticalAlignment::Bottom:
        rectToAlign.top = (std::max)(layoutRect.bottom - this->size.y, layoutRect.top);
        rectToAlign.bottom = layoutRect.bottom;
        break;
    case WidgetVerticalAlignment::Stretch:
        rectToAlign.top = layoutRect.top;
        rectToAlign.bottom = layoutRect.bottom;
        break;
    default:
        assert(false);
        break;
    }

    return rectToAlign;
}

D2D1_RECT_F WidgetBase::HorizontalAlign(const D2D1_RECT_F& layoutRect, D2D1_RECT_F rectToAlign /*= {}*/) const {
    switch (this->horzAlign) {
    case WidgetHorizontalAlignment::Left:
        rectToAlign.left = layoutRect.left;
        rectToAlign.right = (std::min)(layoutRect.left + this->size.x, layoutRect.right);
        break;
    case WidgetHorizontalAlignment::Center: {
        float halfWidth = this->size.x / 2.f;
        float centerX = (layoutRect.left + layoutRect.right) / 2.f;

        rectToAlign.left = (std::max)(centerX - halfWidth, layoutRect.left);
        rectToAlign.right = (std::min)(centerX + halfWidth, layoutRect.right);

        break;
    }
    case WidgetHorizontalAlignment::Right:
        rectToAlign.left = (std::max)(layoutRect.right - this->size.x, layoutRect.left);
        rectToAlign.right = layoutRect.right;
        break;
    case WidgetHorizontalAlignment::Stretch:
        rectToAlign.left = layoutRect.left;
        rectToAlign.right = layoutRect.right;
        break;
    default:
        assert(false);
        break;
    }

    return rectToAlign;
}

WidgetMargin WidgetBase::MarginToPixelValue(WidgetMargin margin, const DirectX::XMFLOAT2& layoutSize) {
    margin.left = WidgetBase::MarginValueToPixelValue(margin.left, layoutSize.x);
    margin.top = WidgetBase::MarginValueToPixelValue(margin.top, layoutSize.y);
    margin.right = WidgetBase::MarginValueToPixelValue(margin.right, layoutSize.x);
    margin.bottom = WidgetBase::MarginValueToPixelValue(margin.bottom, layoutSize.y);

    return margin;
}

WidgetMarginValue WidgetBase::MarginValueToPixelValue(WidgetMarginValue value, float size) {
    if (value.type == WidgetMarginValueType::Normalized) {
        value.value = value.value * size;
    }

    return value;
}
