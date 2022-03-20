#pragma once
#include "IFilesystemProgress.h"

#include <string_view>

namespace Filesystem {
    class IFilesystem {
    public:
        virtual ~IFilesystem() = default;

        virtual void CopyFile(const std::wstring_view dst, const std::wstring_view src) = 0;
        virtual void DeleteFile(const std::wstring_view path) = 0;
        virtual void DeleteFolder(const std::wstring_view path, IFilesystemProgress* progress = nullptr) = 0;
    };
}
