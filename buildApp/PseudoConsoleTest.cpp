#include "PseudoConsoleTest.h"

#include <Windows.h>
#include <memory>
#include <vector>
#include <string>
#include <string_view>

namespace PseudoConsoleTest {
    HRESULT InitializeStartupInfoAttachedToConPTY(STARTUPINFOEXW* siEx, HPCON hPC, std::unique_ptr<BYTE[]>& attrList)
    {
        HRESULT hr = E_UNEXPECTED;
        size_t size;

        siEx->StartupInfo.cb = sizeof(STARTUPINFOEXW);

        // Create the appropriately sized thread attribute list
        InitializeProcThreadAttributeList(NULL, 1, 0, &size);
        attrList = std::make_unique<BYTE[]>(size);

        // Set startup info's attribute list & initialize it
        siEx->lpAttributeList = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(
            attrList.get());
        bool fSuccess = InitializeProcThreadAttributeList(
            siEx->lpAttributeList, 1, 0, (PSIZE_T)&size);

        if (fSuccess)
        {
            // Set thread attribute list's Pseudo Console to the specified ConPTY
            fSuccess = UpdateProcThreadAttribute(
                siEx->lpAttributeList,
                0,
                PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                hPC,
                sizeof(HPCON),
                NULL,
                NULL);
            return fSuccess ? S_OK : HRESULT_FROM_WIN32(GetLastError());
        }
        else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
        return hr;
    }

    HRESULT AttachPseudoConsole(HPCON hPC, LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList)
    {
        BOOL fSuccess = UpdateProcThreadAttribute(lpAttributeList,
            0,
            PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
            hPC,
            sizeof(HPCON),
            nullptr,
            nullptr);
        return fSuccess ? S_OK : HRESULT_FROM_WIN32(GetLastError());
    }

    std::vector<char> ReadTestFile() {
        auto hfile = CreateFileW(L"testOut.bin", GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        DWORD fileSize = GetFileSize(hfile, nullptr);

        std::vector<char> data;

        data.resize(fileSize);

        ReadFile(hfile, data.data(), fileSize, nullptr, nullptr);

        CloseHandle(hfile);

        return data;
    }

    void Test() {
        //HANDLE hOut, hIn;
        HANDLE outPipeOurSide, inPipeOurSide;
        HANDLE outPipePseudoConsoleSide, inPipePseudoConsoleSide;
        HPCON hPC = 0;
        HRESULT hr = S_OK;
        BOOL fSuccess = FALSE;

        // Create the in/out pipes:
        fSuccess = CreatePipe(&inPipePseudoConsoleSide, &inPipeOurSide, nullptr, 0);
        fSuccess = CreatePipe(&outPipeOurSide, &outPipePseudoConsoleSide, nullptr, 0);

        // Create the Pseudo Console, using the pipes
        hr = CreatePseudoConsole(
            { 120, 30 },
            inPipePseudoConsoleSide,
            outPipePseudoConsoleSide,
            0,
            &hPC);

        STARTUPINFOEXW siEx;
        siEx = { 0 };
        siEx.StartupInfo.cb = sizeof(STARTUPINFOEXW);
        size_t size;
        InitializeProcThreadAttributeList(nullptr, 1, 0, (PSIZE_T)&size);
        std::unique_ptr<BYTE[]> attrList = std::make_unique<BYTE[]>(size);
        siEx.lpAttributeList = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(attrList.get());
        fSuccess = InitializeProcThreadAttributeList(siEx.lpAttributeList, 1, 0, (PSIZE_T)&size);

        hr = AttachPseudoConsole(hPC, siEx.lpAttributeList);

        // Prepare the StartupInfoEx structure attached to the ConPTY.
        /*STARTUPINFOEXW siEx{};
        InitializeStartupInfoAttachedToConPTY(&siEx, hPC);*/

        // Create the client application, using startup info containing ConPTY info
        //std::wstring commandline = L"cmd.exe /k \"ping google.com\"";
        std::wstring commandline = L"ping google.com";

        std::vector<wchar_t> cmdVec(commandline.begin(), commandline.end());
        cmdVec.push_back(0);

        PROCESS_INFORMATION piClient{};
        fSuccess = CreateProcessW(
            nullptr,
            cmdVec.data(),
            nullptr,
            nullptr,
            TRUE,
            EXTENDED_STARTUPINFO_PRESENT,
            nullptr,
            nullptr,
            &siEx.StartupInfo,
            &piClient);

        std::string result;

        fSuccess = SetConsoleOutputCP(CP_UTF8);

        HANDLE hOut = NULL;

        hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFOEX csbiex = { 0 };
        csbiex.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
        fSuccess = GetConsoleScreenBufferInfoEx(hOut, &csbiex);

        DWORD dwMode = 0;
        fSuccess = GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        dwMode |= DISABLE_NEWLINE_AUTO_RETURN;
        fSuccess = SetConsoleMode(hOut, dwMode);

        auto testData = ReadTestFile();

        std::string ss = testData.data();
        auto dataOffset = (std::min)(390, (int)testData.size());
        auto dataStart = testData.data() + dataOffset;
        auto dataSize = (int)testData.size() - dataOffset;
        auto dataView = std::string_view(testData.data() + dataOffset + 6, testData.data() + dataOffset + dataSize - 469);

        WriteFile(hOut, dataView.data(), (DWORD)dataView.size(), nullptr, nullptr);
        //WriteFile(hOut, testData.data(), testData.size(), nullptr, nullptr);

        /*auto hfile = CreateFileW(L"testOut.bin", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);

        while (true) {
            char buf[4096] = {};
            DWORD read = 0;

            if (!ReadFile(outPipeOurSide, buf, ARRAYSIZE(buf), &read, nullptr)) {
                break;
            }

            fSuccess = WriteFile(hOut, buf, read, nullptr, nullptr);
            fSuccess = WriteFile(hfile, buf, read, nullptr, nullptr);

            result.insert(result.end(), buf, buf + read);
            int stop = 234;
        }*/

        auto err = GetLastError();
        int stop = 324;

        while (true)
        {

        }
    }
}
