#pragma once
#include "IFilesystem.h"

#include <string>

namespace Filesystem {
    class FilesystemWin : public IFilesystem {
    public:
        FilesystemWin(std::wstring baseFolder);

        void CopyFile(const std::wstring_view dst, const std::wstring_view src) override;
        void DeleteFile(const std::wstring_view path) override;

    private:
        std::wstring baseFolder;
    };
}
