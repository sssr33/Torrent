#include "TextWidget.h"

#include <Helpers/HResultException.h>

void TextWidget::Draw(WidgetRenderContext& ctx) {
    auto d2dCtx = ctx.dev.GetD2DContext();
    auto dwrite = ctx.dev.GetDwriteFactory();

    if (!this->textFormat) {
        HRESULT hr = S_OK;

        hr = dwrite->CreateTextFormat(
            fontFamilyName.c_str(),
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            this->fontSize,
            this->localeName.c_str(),
            &this->textFormat);
        Helpers::ThrowIfFailed(hr);

        // need new textLayout when textFormat changed
        this->textLayout = nullptr;
    }

    auto actualRect = this->GetActualRect();
    if (!actualRect) {
        return;
    }

    if (!this->textLayout) {
        HRESULT hr = S_OK;

        auto size = D2D1::SizeF(
            actualRect->right - actualRect->left,
            actualRect->bottom - actualRect->top);

        hr = dwrite->CreateTextLayout(
            this->text.c_str(),
            static_cast<UINT32>(this->text.size()),
            this->textFormat.Get(),
            size.width,
            size.height,
            &this->textLayout);
        Helpers::ThrowIfFailed(hr);

        hr = this->textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        Helpers::ThrowIfFailed(hr);

        hr = this->textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        Helpers::ThrowIfFailed(hr);
    }

    if (!this->textColorBrush) {
        HRESULT hr = S_OK;

        hr = d2dCtx->CreateSolidColorBrush(this->textColor, &this->textColorBrush);
        Helpers::ThrowIfFailed(hr);
    }

    d2dCtx->DrawTextLayout(
        D2D1::Point2F(actualRect->left, actualRect->top),
        this->textLayout.Get(),
        this->textColorBrush.Get());

    TextWidgetBase::DrawChildren(ctx);
}

void TextWidget::SetMargin(const WidgetMargin& margin) {
    TextWidgetBase::SetMargin(margin);
    this->textLayout = nullptr;
}

void TextWidget::SetSize(const DirectX::XMFLOAT2& size) {
    TextWidgetBase::SetSize(size);
    this->textLayout = nullptr;
}

void TextWidget::SetVerticalAlignment(const WidgetVerticalAlignment& vertAlign) {
    TextWidgetBase::SetVerticalAlignment(vertAlign);
    this->textLayout = nullptr;
}

void TextWidget::SetHorizontalAlignment(const WidgetHorizontalAlignment& horzAlign) {
    TextWidgetBase::SetHorizontalAlignment(horzAlign);
    this->textLayout = nullptr;
}

std::wstring TextWidget::GetText() const {
    return this->text;
}

void TextWidget::SetText(const std::wstring& text) {
    this->text = text;
    this->textLayout = nullptr;
}

std::wstring TextWidget::GetFontFamilyName() const {
    return this->fontFamilyName;
}

void TextWidget::SetFontFamilyName(const std::wstring& fontFamilyName) {
    this->fontFamilyName = fontFamilyName;
    this->textFormat = nullptr;
}

std::wstring TextWidget::GetLocaleName() const {
    return this->localeName;
}

void TextWidget::SetLocaleName(const std::wstring& localeName) {
    this->localeName = localeName;
    this->textFormat = nullptr;
}

float TextWidget::GetFontSize() const {
    return this->fontSize;
}

void TextWidget::SetFontSize(float fontSize) {
    this->fontSize = fontSize;
    this->textFormat = nullptr;
}

D2D1_COLOR_F TextWidget::GetTextColor() const {
    return this->textColor;
}

void TextWidget::SetTextColor(const D2D1_COLOR_F& textColor) {
    this->textColor = textColor;
    this->textColorBrush = nullptr;
}
