#pragma once
#include "WidgetRenderContext.h"
#include "WidgetVerticalAlignment.h"
#include "WidgetHorizontalAlignment.h"
#include "WidgetMargin.h"
#include "WidgetAttachedPropertyValue.h"

#include <memory>
#include <cstdint>
#include <string>
#include <string_view>
#include <optional>

class IWidget : public std::enable_shared_from_this<IWidget> {
public:
    virtual ~IWidget() = default;

    //virtual void Layout() = 0;
    virtual void Draw(WidgetRenderContext& ctx) = 0;

    virtual WidgetMargin GetMargin() = 0;
    virtual void SetMargin(const WidgetMargin& margin) = 0;

    virtual DirectX::XMFLOAT2 GetSize() = 0;
    virtual void SetSize(const DirectX::XMFLOAT2& size) = 0;

    virtual WidgetVerticalAlignment GetVerticalAlignment() = 0;
    virtual void SetVerticalAlignment(const WidgetVerticalAlignment& vertAlign) = 0;

    virtual WidgetHorizontalAlignment GetHorizontalAlignment() = 0;
    virtual void SetHorizontalAlignment(const WidgetHorizontalAlignment& horzAlign) = 0;

    virtual std::optional<D2D1_RECT_F> GetActualRect() = 0;

    virtual WidgetAttachedPropertyValue GetAttachedProperty(const std::string& name) = 0;
    virtual void SetAttachedProperty(const std::string& name, WidgetAttachedPropertyValue value) = 0;

    virtual std::shared_ptr<IWidget> GetParent() = 0;
    // assumed to be used in AddChild
    virtual void SetParent(std::weak_ptr<IWidget> parent) = 0;

    virtual std::optional<D2D1_RECT_F> GetChildLayoutRect(IWidget* child) = 0;

    virtual void AddChild(std::shared_ptr<IWidget> widget) = 0;
    virtual void RemoveChild(std::shared_ptr<IWidget> widget) = 0;

    /*virtual DirectX::XMFLOAT2 GetLeftTop() = 0;
    virtual void SetLeftTop(const DirectX::XMFLOAT2& leftTop) = 0;

    virtual DirectX::XMFLOAT2 GetSize() = 0;
    virtual void SetSize(const DirectX::XMFLOAT2& size) = 0;

    D2D1_RECT_F GetRect();*/

    /*virtual std::string GetName() = 0;
    virtual void SetName(std::string name) = 0;

    virtual std::shared_ptr<IWidget> GetParent() = 0;

    virtual void AddChild(std::shared_ptr<IWidget> widget) = 0;
    virtual void RemoveChild(std::shared_ptr<IWidget> widget) = 0;
    virtual void RemoveChild(uint32_t idx) = 0;
    virtual void RemoveChild(const std::string& name) = 0;
    virtual std::shared_ptr<IWidget> GetChild(uint32_t idx) = 0;
    virtual std::shared_ptr<IWidget> GetChild(const std::string& name) = 0;
    virtual uint32_t GetChildrenCount() = 0;*/
};
