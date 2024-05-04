#include "Window.h"

#include <bit>
#include <stdexcept>
#include <cassert>

#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")

struct ACCENTPOLICY
{
    int na;
    int nf;
    int nc;
    int nA;
};
struct WINCOMPATTRDATA
{
    int na;
    PVOID pd;
    ULONG ul;
};

typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

void makeBlur(HWND hwnd)
{
    const HINSTANCE hm = LoadLibraryW(L"user32.dll");
    if (hm)
    {
        const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hm, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute)
        {
            struct ACCENTPOLICY policy = { 3, 0, 0, 0 };
            struct WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
        FreeLibrary(hm);
    }
}

Window::Window(std::wstring windowClassName, std::wstring windowName, bool visibleOnCreate)
    : windowClassName(std::move(windowClassName))
    , windowName(std::move(windowName))
{
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &Window::WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wc.lpszClassName = this->windowClassName.c_str();

    if (!RegisterClass(&wc)) {
        DWORD errorCode = GetLastError();

        if (errorCode != ERROR_CLASS_ALREADY_EXISTS) {
            throw std::runtime_error("RegisterClass failed");
        }
    }

    // TODO try to add Acrylic Effect
    // https://stackoverflow.com/questions/44000217/mimicking-acrylic-in-a-win32-app
    // https://github.com/selastingeorge/Win32-Acrylic-Effect
    // https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.ui.composition.systembackdrops.desktopacryliccontroller?view=windows-app-sdk-1.1
    // https://stackoverflow.com/questions/70231916/how-to-implement-acrylic-windows-effect-in-winui3-in-desktop-application
    // https://habr.com/ru/articles/658067/

    // https://stackoverflow.com/questions/38179033/when-should-ws-ex-noredirectionbitmap-be-used
    // https://learn.microsoft.com/en-us/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine
    // to remove border https://stackoverflow.com/questions/39731497/create-window-without-titlebar-with-resizable-border-and-without-bogus-6px-whit
    this->hwnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP /*| WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST*/,
        this->windowClassName.c_str(),
        this->windowName.c_str(),
        WS_OVERLAPPEDWINDOW,
        //WS_POPUP | WS_THICKFRAME | WS_BORDER | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, // to remove border
        // Size and position
        //CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        200, 200, 1280, 720,
        nullptr,       // Parent window
        nullptr,       // Menu
        wc.hInstance,
        this
    );

    /*if (!SetLayeredWindowAttributes(this->hwnd, 0, 255, LWA_ALPHA)) {
        assert(false);
    }*/

    if (!this->hwnd) {
        throw std::runtime_error("CreateWindowEx failed");
    }

    //{
    //    HRESULT hr = S_OK;

    //    // Create and populate the Blur Behind structure
    //    DWM_BLURBEHIND bb = { 0 };

    //    // Enable Blur Behind and apply to the entire client area
    //    bb.dwFlags = DWM_BB_ENABLE;
    //    bb.fEnable = true;
    //    bb.hRgnBlur = NULL;

    //    // Apply Blur Behind
    //    hr = DwmEnableBlurBehindWindow(this->hwnd, &bb);
    //    int stop = 234;
    //}

    //makeBlur(this->hwnd);

    if (visibleOnCreate) {
        ShowWindow(this->hwnd, SW_SHOW);
    }
}

Window::~Window() {
    if (!this->destroyReceived) {
        BOOL result = DestroyWindow(hwnd);

        if (result == FALSE) {
            DWORD errorCode = GetLastError();
            // TODO check logic
            assert(false);
        }
    }
}

HWND Window::GetHwnd() const {
    return this->hwnd;
}

Window::CustomWndProcHandler Window::GetCustomWndProcHandler() const {
    return this->customWndProcHandler;
}

void Window::SetCustomWndProcHandler(CustomWndProcHandler handler) {
    this->customWndProcHandler = std::move(handler);
}

bool Window::GetVisible() const {
    bool visible = IsWindowVisible(this->hwnd) != FALSE;
    return visible;
}

void Window::SetVisible(bool visible) {
    ShowWindow(this->hwnd, visible ? SW_SHOW : SW_HIDE);
}

WindowsThreadMessageQueue Window::MakeWindowsThreadMessageQueue() {
    return WindowsThreadMessageQueue();
}

LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LONG_PTR userData = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    auto wnd = std::bit_cast<Window*>(userData);

    switch (uMsg) {
    case WM_CREATE: {
        auto createStruct = std::bit_cast<CREATESTRUCT*>(lParam);
        auto wndTmp = static_cast<Window*>(createStruct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(wndTmp));
        return 0;
    }
    case WM_DESTROY:
        if (wnd) {
            wnd->destroyReceived = true;
        }
        PostQuitMessage(0);
        return 0;
    default: {
        if (wnd && wnd->customWndProcHandler) {
            return wnd->customWndProcHandler(wnd, hwnd, uMsg, wParam, lParam);
        }
        else {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    }

    return 0;
}
