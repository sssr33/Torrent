#pragma once
#include "IWidget.h"

#include <map>

class WidgetBase : public IWidget {
public:
    WidgetMargin GetMargin() override;
    void SetMargin(const WidgetMargin& margin) override;

    DirectX::XMFLOAT2 GetSize() override;
    void SetSize(const DirectX::XMFLOAT2& size) override;

    WidgetVerticalAlignment GetVerticalAlignment() override;
    void SetVerticalAlignment(const WidgetVerticalAlignment& vertAlign) override;

    WidgetHorizontalAlignment GetHorizontalAlignment() override;
    void SetHorizontalAlignment(const WidgetHorizontalAlignment& horzAlign) override;

    std::optional<D2D1_RECT_F> GetActualRect() override;

    WidgetAttachedPropertyValue GetAttachedProperty(const std::string& name) override;
    void SetAttachedProperty(const std::string& name, WidgetAttachedPropertyValue value) override;

    std::shared_ptr<IWidget> GetParent() override;
    void SetParent(std::weak_ptr<IWidget> parent) override;

    std::optional<D2D1_RECT_F> GetChildLayoutRect(IWidget* child) override;

private:
    D2D1_RECT_F VerticalAlign(const D2D1_RECT_F& layoutRect, D2D1_RECT_F rectToAlign = {}) const;
    D2D1_RECT_F HorizontalAlign(const D2D1_RECT_F& layoutRect, D2D1_RECT_F rectToAlign = {}) const;

    static WidgetMargin MarginToPixelValue(WidgetMargin margin, const DirectX::XMFLOAT2& layoutSize);
    static WidgetMarginValue MarginValueToPixelValue(WidgetMarginValue value, float size);

    WidgetMargin margin;
    DirectX::XMFLOAT2 size = { 0.f, 0.f };
    WidgetVerticalAlignment vertAlign = WidgetVerticalAlignment::Center;
    WidgetHorizontalAlignment horzAlign = WidgetHorizontalAlignment::Center;
    std::map<std::string, WidgetAttachedPropertyValue> attachedProps;
    std::weak_ptr<IWidget> parent;
};
