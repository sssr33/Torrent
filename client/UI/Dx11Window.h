#pragma once
#include "Window.h"
#include "Dx11Device.h"
#include "Dx11HwndSwapChain.h"

class Dx11Window {
public:
    Dx11Window(std::wstring windowClassName, std::wstring windowName);

private:
    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    Window wnd;
    Dx11Device dxDev;
    Dx11HwndSwapChain dxSwapChain;
};
