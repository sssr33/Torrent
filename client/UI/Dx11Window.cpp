#include "Dx11Window.h"

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
    try {
        switch (uMsg) {
        case WM_SIZE: {
            auto width = LOWORD(lParam);
            auto height = HIWORD(lParam);
            this->dxSwapChain.Resize(
                DirectX::XMUINT2(width, height),
                this->dxDev.GetD3DDevice(),
                this->dxDev.GetD3DContext(),
                this->dxDev.GetD2DContext());
            return 0;
        }
        /*case WM_PAINT: {
            return 0;
        }*/
        }
    }
    catch (...) {
        // check logic
        assert(false);
    }

    return DefWindowProc(this->wnd.GetHwnd(), uMsg, wParam, lParam);
}
