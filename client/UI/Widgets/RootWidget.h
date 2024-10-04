#pragma once
#include "WidgetDefaultChildContainer.h"

using RootWidgetBase = WidgetDefaultChildContainer<IWidget>;

class RootWidget final : public RootWidgetBase {
public:
    void Draw(WidgetRenderContext& ctx) override;

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
    DirectX::XMFLOAT2 size = { 0.f, 0.f };
};
