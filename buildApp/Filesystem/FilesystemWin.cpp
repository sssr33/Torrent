#include "FilesystemWin.h"

#include <Windows.h>
#include <wrl.h>
#include <Shobjidl.h>
#include <algorithm>
#include <vector>

#undef CopyFile
#undef DeleteFile

namespace Filesystem {
    class FilesystemWinProgress : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>, IFileOperationProgressSink>
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE StartOperations(void) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE FinishOperations(
            /* [in] */ HRESULT hrResult) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PreRenameItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszNewName) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PostRenameItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem,
            /* [string][in] */ __RPC__in_string LPCWSTR pszNewName,
            /* [in] */ HRESULT hrRename,
            /* [in] */ __RPC__in_opt IShellItem* psiNewlyCreated) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PreMoveItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem,
            /* [in] */ __RPC__in_opt IShellItem* psiDestinationFolder,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszNewName) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PostMoveItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem,
            /* [in] */ __RPC__in_opt IShellItem* psiDestinationFolder,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszNewName,
            /* [in] */ HRESULT hrMove,
            /* [in] */ __RPC__in_opt IShellItem* psiNewlyCreated) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PreCopyItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem,
            /* [in] */ __RPC__in_opt IShellItem* psiDestinationFolder,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszNewName) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PostCopyItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem,
            /* [in] */ __RPC__in_opt IShellItem* psiDestinationFolder,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszNewName,
            /* [in] */ HRESULT hrCopy,
            /* [in] */ __RPC__in_opt IShellItem* psiNewlyCreated) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PreDeleteItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PostDeleteItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiItem,
            /* [in] */ HRESULT hrDelete,
            /* [in] */ __RPC__in_opt IShellItem* psiNewlyCreated) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PreNewItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiDestinationFolder,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszNewName) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PostNewItem(
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IShellItem* psiDestinationFolder,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszNewName,
            /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszTemplateName,
            /* [in] */ DWORD dwFileAttributes,
            /* [in] */ HRESULT hrNew,
            /* [in] */ __RPC__in_opt IShellItem* psiNewItem) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE UpdateProgress(
            /* [in] */ UINT iWorkTotal,
            /* [in] */ UINT iWorkSoFar) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE ResetTimer(void) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE PauseTimer(void) override {
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE ResumeTimer(void) override {
            return S_OK;
        }
    };

    FilesystemWin::FilesystemWin(std::wstring baseFolder)
        : baseFolder(FixPathForWindows(baseFolder))
    {

    }

    void FilesystemWin::CopyFile(const std::wstring_view dst, const std::wstring_view src) {
        auto dstPath = this->GetFullPath(dst);
        auto srcPath = this->GetFullPath(src);
        CopyFileW(srcPath.c_str(), dstPath.c_str(), FALSE);
    }

    void FilesystemWin::DeleteFile(const std::wstring_view path) {
        auto filePath = this->GetFullPath(path);
        DeleteFileW(filePath.c_str());
    }

    void FilesystemWin::DeleteFolder(const std::wstring_view path, IFilesystemProgress* progress) {
        auto folderPath = this->GetFullPath(path);

        DWORD bufSize = GetFullPathNameW(folderPath.c_str(), 0, nullptr, nullptr);
        if (!bufSize) {
            return;
        }

        std::vector<wchar_t> buffer;
        buffer.resize(bufSize, L'\0');

        if (!GetFullPathNameW(folderPath.c_str(), static_cast<DWORD>(buffer.size()), buffer.data(), nullptr)) {
            return;
        }

        HRESULT hr = S_OK;
        Microsoft::WRL::ComPtr<IShellItem> folder;

        hr = SHCreateItemFromParsingName(buffer.data(), nullptr, IID_PPV_ARGS(folder.GetAddressOf()));
        if (FAILED(hr)) {
            return;
        }

        Microsoft::WRL::ComPtr<IFileOperation> fileOp;

        hr = CoCreateInstance(__uuidof(FileOperation), NULL, CLSCTX_ALL, IID_PPV_ARGS(fileOp.GetAddressOf()));
        if (FAILED(hr)) {
            return;
        }

        hr = fileOp->SetOperationFlags(FOF_NO_UI);
        if (FAILED(hr)) {
            return;
        }

        auto progressTest = Microsoft::WRL::Make<FilesystemWinProgress>();

        hr = fileOp->DeleteItem(folder.Get(), progressTest.Get());
        if (FAILED(hr)) {
            return;
        }

        hr = fileOp->PerformOperations();
        if (FAILED(hr)) {
            return;
        }

        return;
    }

    std::wstring FilesystemWin::GetFullPath(const std::wstring_view relative) const {
        return this->baseFolder + L'\\' + FixPathForWindows(relative);
    }

    std::wstring FilesystemWin::FixPathForWindows(const std::wstring_view path) {
        return RemoveTrailingSlashes(ReplaceSlashesToWindows(path));
    }

    std::wstring FilesystemWin::ReplaceSlashesToWindows(const std::wstring_view path) {
        auto result = std::wstring(path);
        std::replace(result.begin(), result.end(), L'/', L'\\');
        return result;
    }

    std::wstring FilesystemWin::RemoveTrailingSlashes(const std::wstring_view path) {
        if (path.empty()) {
            return std::wstring();
        }

        auto begin = path.begin();
        auto end = path.end();
        auto beforeEnd = end - 1;

        if (IsSlash(*begin)) {
            ++begin;
        }

        if (begin != end && IsSlash(*(end - 1))) {
            --end;
        }

        return std::wstring(begin, end);
    }

    bool FilesystemWin::IsSlash(wchar_t ch) {
        return ch == L'\\' || ch == L'/';
    }
}
