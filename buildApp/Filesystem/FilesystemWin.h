#pragma once
#include "IFilesystem.h"

#include <string>

namespace Filesystem {
    class FilesystemWin : public IFilesystem {
    public:
        FilesystemWin(std::wstring baseFolder);

        void CopyFile(const std::wstring_view dst, const std::wstring_view src) override;
        void DeleteFile(const std::wstring_view path) override;

        void CreateFolder(const std::wstring_view path) override;
        void DeleteFolder(const std::wstring_view path, IFilesystemProgress* progress = nullptr) override;

    private:
        std::wstring GetFullPath(const std::wstring_view relative) const;
        static std::wstring FixPathForWindows(const std::wstring_view path);
        static std::wstring ReplaceSlashesToWindows(const std::wstring_view path);
        static std::wstring RemoveTrailingSlashes(const std::wstring_view path);
        static bool IsSlash(wchar_t ch);

        std::wstring baseFolder;
    };
}
