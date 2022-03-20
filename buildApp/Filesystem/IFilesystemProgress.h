#pragma once

namespace Filesystem {
    class IFilesystemProgress {
    public:
        virtual ~IFilesystemProgress() = default;

        // <progress> from 0.f to 1.f
        virtual void ReportProgress(float progress) = 0;
    };
}
