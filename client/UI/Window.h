#pragma once
#include "WindowsThreadMessageQueue.h"

#include <string>
#include <Windows.h>
#include <functional>

// if no title and no default controls has to be implemented
// https://stackoverflow.com/questions/7442939/opening-a-window-that-has-no-title-bar-with-win32
class Window {
public:
    using CustomWndProcHandler = std::function<LRESULT(Window* window, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>;

    Window(std::wstring windowClassName, std::wstring windowName, bool visibleOnCreate);

    Window(const Window&) = delete;
    Window(Window&&) = delete;

    ~Window();

    HWND GetHwnd() const;

    CustomWndProcHandler GetCustomWndProcHandler() const;
    void SetCustomWndProcHandler(CustomWndProcHandler handler);

    bool GetVisible() const;
    void SetVisible(bool visible);

    // WindowsThreadMessageQueue will process all messages for all windows where this window created
    // WindowsThreadMessageQueue can be created by instantiating WindowsThreadMessageQueue
    // This method just a hint on how to receive messages for Window class
    static WindowsThreadMessageQueue MakeWindowsThreadMessageQueue();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    WNDCLASS wc = {};
    HWND hwnd = nullptr;
    std::wstring windowClassName;
    std::wstring windowName;
    bool destroyReceived = false;
    CustomWndProcHandler customWndProcHandler;
};
