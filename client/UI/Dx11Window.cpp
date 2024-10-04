#include "Dx11Window.h"
#include "Widgets/RootWidget.h"
#include "Widgets/GridWidget.h"
#include "Widgets/RectangleWidget.h"
#include "Widgets/TextWidget.h"

#include <cassert>

Dx11Window::Dx11Window(std::wstring windowClassName, std::wstring windowName)
    : wnd(std::move(windowClassName), std::move(windowName), false)
    , dxSwapChain(this->wnd.GetHwnd(), this->dxDev.GetD3DDevice(), this->dxDev.GetD2DContext())
{
    wnd.SetCustomWndProcHandler(
        [this](Window* window, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            return this->WndProc(uMsg, wParam, lParam);
        });

    wnd.SetVisible(true);
}

LRESULT Dx11Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto testRedraw = [&]
    {
        auto d2dTarget = this->dxSwapChain.GetD2DRenderTargetBitmap();
        auto d2dCtx = this->dxDev.GetD2DContext();

        d2dCtx->BeginDraw();
        d2dCtx->SetTarget(d2dTarget.Get());

        auto rootWidget = std::make_shared<RootWidget>();
        auto rectangleWidget = std::make_shared<RectangleWidget>();

        auto sz = d2dTarget->GetSize();

        rootWidget->SetSize(DirectX::XMFLOAT2(sz.width, sz.height));

        rootWidget->AddChild(rectangleWidget);

        rectangleWidget->SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::BlueViolet, 0.3f));
        rectangleWidget->SetVerticalAlignment(WidgetVerticalAlignment::Stretch);
        rectangleWidget->SetHorizontalAlignment(WidgetHorizontalAlignment::Stretch);

        {
            auto gridWidget = std::make_shared<GridWidget>();

            gridWidget->SetVerticalAlignment(WidgetVerticalAlignment::Stretch);
            gridWidget->SetHorizontalAlignment(WidgetHorizontalAlignment::Stretch);

            {
                std::vector<GridWidget::RowDefinition> rows;
                {
                    GridWidget::RowDefinition row;

                    row.height = 1.f;
                    row.sizeType = GridWidget::SizeType::Weighted;

                    rows.push_back(row);
                }

                gridWidget->SetRows(std::move(rows));
            }

            {
                std::vector<GridWidget::ColumnDefinition> columns;
                {
                    GridWidget::ColumnDefinition col;

                    col.width = 10.f;
                    col.sizeType = GridWidget::SizeType::Pixel;

                    columns.push_back(col);
                }
                {
                    GridWidget::ColumnDefinition col;

                    col.width = 2.f;
                    col.sizeType = GridWidget::SizeType::Weighted;

                    columns.push_back(col);
                }
                {
                    GridWidget::ColumnDefinition col;

                    /*col.width = 2.f;
                    col.sizeType = GridWidget::SizeType::Weighted;*/

                    col.width = 10.f;
                    col.sizeType = GridWidget::SizeType::Pixel;

                    columns.push_back(col);
                }

                gridWidget->SetColumns(std::move(columns));
            }

            {
                auto rectangleCellWidget = std::make_shared<RectangleWidget>();

                rectangleCellWidget->SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::Red, 0.3f));
                rectangleCellWidget->SetVerticalAlignment(WidgetVerticalAlignment::Stretch);
                rectangleCellWidget->SetHorizontalAlignment(WidgetHorizontalAlignment::Stretch);

                GridWidget::SetWidgetGridRowIdx(*rectangleCellWidget, 0);
                GridWidget::SetWidgetGridColumnIdx(*rectangleCellWidget, 0);

                gridWidget->AddChild(rectangleCellWidget);
            }
            {
                auto textCellWidget = std::make_shared<TextWidget>();

                textCellWidget->SetFontSize(54.f);
                textCellWidget->SetText(L"This is text widget 😱");
                textCellWidget->SetTextColor(D2D1::ColorF(D2D1::ColorF::Yellow, 0.3f));
                textCellWidget->SetVerticalAlignment(WidgetVerticalAlignment::Stretch);
                textCellWidget->SetHorizontalAlignment(WidgetHorizontalAlignment::Stretch);

                GridWidget::SetWidgetGridRowIdx(*textCellWidget, 0);
                GridWidget::SetWidgetGridColumnIdx(*textCellWidget, 1);

                gridWidget->AddChild(textCellWidget);
            }
            {
                auto rectangleCellWidget = std::make_shared<RectangleWidget>();

                rectangleCellWidget->SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::Green, 0.3f));
                rectangleCellWidget->SetVerticalAlignment(WidgetVerticalAlignment::Stretch);
                rectangleCellWidget->SetHorizontalAlignment(WidgetHorizontalAlignment::Stretch);

                GridWidget::SetWidgetGridRowIdx(*rectangleCellWidget, 0);
                GridWidget::SetWidgetGridColumnIdx(*rectangleCellWidget, 2);

                gridWidget->AddChild(rectangleCellWidget);
            }

            rootWidget->AddChild(gridWidget);
        }

        WidgetRenderContext ctx = { this->dxDev };

        d2dCtx->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));

        rootWidget->Draw(ctx);


        /*d2dCtx->Clear(D2D1::ColorF(D2D1::ColorF::BlueViolet, 0.3f));

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

        auto greenBrush = d2dCtx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSeaGreen, 0.5f), &brush);

        d2dCtx->DrawLine(D2D1::Point2F(100.f, 100.f), D2D1::Point2F(400.f, 200.f), brush.Get(), 10.f);*/

        d2dCtx->EndDraw();

        this->dxSwapChain.Present(this->dxDev.GetD3DContext());
    };

    try {
        switch (uMsg) {
        // https://stackoverflow.com/questions/21816323/smooth-window-resizing-in-windows-using-direct2d-1-1
        // https://stackoverflow.com/questions/53000291/how-to-smooth-ugly-jitter-flicker-jumping-when-resizing-windows-especially-drag
        // https://github.com/bigfatbrowncat/noflicker_directx_window
        case WM_NCCALCSIZE: {
            // Use the result of DefWindowProc's WM_NCCALCSIZE handler to get the upcoming client rect.
            // Technically, when wparam is TRUE, lparam points to NCCALCSIZE_PARAMS, but its first
            // member is a RECT with the same meaning as the one lparam points to when wparam is FALSE.
            DefWindowProc(this->wnd.GetHwnd(), uMsg, wParam, lParam);
            if (RECT* rect = (RECT*)lParam; rect->right > rect->left && rect->bottom > rect->top) {
                // A real app might want to compare these dimensions with the current swap chain
                // dimensions and skip all this if they're unchanged.
                UINT width = rect->right - rect->left;
                UINT height = rect->bottom - rect->top;

                this->dxSwapChain.Resize(
                    DirectX::XMUINT2(width, height),
                    this->dxDev.GetD3DDevice(),
                    this->dxDev.GetD3DContext(),
                    this->dxDev.GetD2DContext());
            }

            // test exclude from capture
            if (false) {
                if (!SetWindowDisplayAffinity(this->wnd.GetHwnd(), WDA_EXCLUDEFROMCAPTURE)) {
                    assert(false);
                }
            }

            return 0;
        }
        //case WM_NCPAINT: // can be used to remove border
        case WM_ERASEBKGND:
        case WM_PAINT: {
            RECT updateRegion = {};
            if (!GetUpdateRect(this->wnd.GetHwnd(), &updateRegion, FALSE)) {
                return 0;
            }

            testRedraw();

            if (!ValidateRect(this->wnd.GetHwnd(), &updateRegion)) {
                assert(false);
            }

            return 0;
        }
        }
    }
    catch (...) {
        // check logic
        assert(false);
    }

    return DefWindowProc(this->wnd.GetHwnd(), uMsg, wParam, lParam);
}
