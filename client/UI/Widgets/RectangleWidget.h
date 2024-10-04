#pragma once
#include "WidgetBase.h"
#include "WidgetDefaultChildContainer.h"

using RectangleWidgetBase = WidgetDefaultChildContainer<WidgetBase>;

class RectangleWidget final : public RectangleWidgetBase {
public:
    void Draw(WidgetRenderContext& ctx) override;

    bool IsBackgroundVisible() const;
    void SetBackgroundVisible(bool visible);

    bool IsBorderVisible() const;
    void SetBorderVisible(bool visible);

    D2D1_COLOR_F GetBackgroundColor() const;
    void SetBackgroundColor(const D2D1_COLOR_F& color);

    D2D1_COLOR_F GetBorderColor() const;
    void SetBorderColor(const D2D1_COLOR_F& color);

    float GetBorderThickness() const;
    void SetBorderThickness(float thickness);

private:
    bool backgroundVisible = true;
    bool borderVisible = false;
    D2D1_COLOR_F backgroundColor = D2D1::ColorF(D2D1::ColorF::Black);
    D2D1_COLOR_F borderColor = D2D1::ColorF(D2D1::ColorF::Black, 0.f);
    float borderThickness = 1.f;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> backgroundBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
};
