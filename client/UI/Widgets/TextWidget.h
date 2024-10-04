#pragma once
#include "WidgetBase.h"
#include "WidgetDefaultChildContainer.h"

#include <string>

using TextWidgetBase = WidgetDefaultChildContainer<WidgetBase>;

class TextWidget final : public TextWidgetBase {
public:
    void Draw(WidgetRenderContext& ctx) override;

    void SetMargin(const WidgetMargin& margin) override;
    void SetSize(const DirectX::XMFLOAT2& size) override;
    void SetVerticalAlignment(const WidgetVerticalAlignment& vertAlign) override;
    void SetHorizontalAlignment(const WidgetHorizontalAlignment& horzAlign) override;

    std::wstring GetText() const;
    void SetText(const std::wstring& text);

    std::wstring GetFontFamilyName() const;
    void SetFontFamilyName(const std::wstring& fontFamilyName);

    std::wstring GetLocaleName() const;
    void SetLocaleName(const std::wstring& text);

    float GetFontSize() const;
    void SetFontSize(float fontSize);

    D2D1_COLOR_F GetTextColor() const;
    void SetTextColor(const D2D1_COLOR_F& textColor);

private:
    std::wstring text;
    std::wstring fontFamilyName = L"Segoe UI";
    std::wstring localeName = L"en_US"; // not working locale - L"en_US.UTF-8"
    float fontSize = 14.f;
    D2D1_COLOR_F textColor = D2D1::ColorF(D2D1::ColorF::Black);

    Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;
    Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textColorBrush;
};
