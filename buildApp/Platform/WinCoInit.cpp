#include "WinCoInit.h"

#include <Windows.h>
#include <stdexcept>

struct WinCoInitHelper {
    WinCoInitHelper() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr)) {
            throw std::exception("CoInitializeEx failed");
        }
    }

    ~WinCoInitHelper() {
        CoUninitialize();
    }
};

WinCoInit::WinCoInit() {
    static WinCoInitHelper init;
}
