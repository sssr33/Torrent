#include "Filesystem/IFilesystem.h"

#include <memory>
#include <string>

namespace Platform {
    class IPlatformFactory {
    public:
        virtual ~IPlatformFactory() = default;

        virtual std::unique_ptr<Filesystem::IFilesystem> CreateFilesystem(std::wstring baseFolder) = 0;
    };
}
