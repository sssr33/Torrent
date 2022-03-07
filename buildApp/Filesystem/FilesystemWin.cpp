#include "FilesystemWin.h"

#include <Windows.h>

#undef CopyFile
#undef DeleteFile

namespace Filesystem {
    FilesystemWin::FilesystemWin(std::wstring baseFolder)
        : baseFolder(std::move(baseFolder))
    {}

    void FilesystemWin::CopyFile(const std::wstring_view dst, const std::wstring_view src) {
        auto dstPath = this->baseFolder + L'/' + std::wstring(dst);
        auto srcPath = this->baseFolder + L'/' + std::wstring(src);
        CopyFileW(srcPath.c_str(), dstPath.c_str(), FALSE);
    }

    void FilesystemWin::DeleteFile(const std::wstring_view path) {
        auto filePath = this->baseFolder + L'/' + std::wstring(path);
        DeleteFileW(filePath.c_str());
    }
}
