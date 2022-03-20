#include "WinPlatformFactory.h"
#include "Filesystem/FilesystemWin.h"
#include "WinCoInit.h"

namespace Platform {
    class WinPlatformFactory : public IPlatformFactory, public WinCoInit {
    public:
        std::unique_ptr<Filesystem::IFilesystem> CreateFilesystem(std::wstring baseFolder) override {
            return std::make_unique<Filesystem::FilesystemWin>(std::move(baseFolder));
        }
    };

    std::unique_ptr<IPlatformFactory> CreateWinPlatformFactory() {
        return std::make_unique<WinPlatformFactory>();
    }
}
