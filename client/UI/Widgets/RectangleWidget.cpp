#include "RectangleWidget.h"
#include "UI/Dx11Operators.h"

#include <Helpers/HResultException.h>

void RectangleWidget::Draw(WidgetRenderContext& ctx) {
    auto d2dCtx = ctx.dev.GetD2DContext();
    auto rectangle = this->GetActualRect();

    if (!rectangle) {
        return;
    }

    if (this->backgroundVisible) {
        if (!this->backgroundBrush) {
            HRESULT hr = S_OK;

            hr = d2dCtx->CreateSolidColorBrush(this->backgroundColor, &this->backgroundBrush);
            Helpers::ThrowIfFailed(hr);
        }

        d2dCtx->FillRectangle(*rectangle, this->backgroundBrush.Get());
    }

    if (this->borderVisible) {
        if (!this->borderBrush) {
            HRESULT hr = S_OK;

            hr = d2dCtx->CreateSolidColorBrush(this->borderColor, &this->borderBrush);
            Helpers::ThrowIfFailed(hr);
        }

        d2dCtx->DrawRectangle(*rectangle, this->borderBrush.Get(), this->borderThickness);
    }

    RectangleWidgetBase::DrawChildren(ctx);
}

bool RectangleWidget::IsBackgroundVisible() const {
    return this->backgroundVisible;
}

void RectangleWidget::SetBackgroundVisible(bool visible) {
    this->backgroundVisible = visible;
}

bool RectangleWidget::IsBorderVisible() const {
    return this->borderVisible;
}

void RectangleWidget::SetBorderVisible(bool visible) {
    this->borderVisible = visible;
}

D2D1_COLOR_F RectangleWidget::GetBackgroundColor() const {
    return this->backgroundColor;
}

void RectangleWidget::SetBackgroundColor(const D2D1_COLOR_F& color) {
    using namespace Dx11Operators;

    if (color == this->backgroundColor) {
        return;
    }

    this->backgroundColor = color;
    this->backgroundBrush = nullptr;
}

D2D1_COLOR_F RectangleWidget::GetBorderColor() const {
    return this->borderColor;
}

void RectangleWidget::SetBorderColor(const D2D1_COLOR_F& color) {
    using namespace Dx11Operators;

    if (color == this->borderColor) {
        return;
    }

    this->borderColor = color;
    this->borderBrush = nullptr;
}

float RectangleWidget::GetBorderThickness() const {
    return this->borderThickness;
}

void RectangleWidget::SetBorderThickness(float thickness) {
    this->borderThickness = thickness;
}
