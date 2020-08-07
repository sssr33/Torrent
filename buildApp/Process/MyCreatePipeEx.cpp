#include "MyCreatePipeEx.h"

#include <stdio.h>

namespace Process
{
    std::atomic<uint32_t> MyCreatePipeEx::PipeSerialNumber = 0;

	BOOL APIENTRY MyCreatePipeEx::operator()(
		OUT LPHANDLE lpReadPipe,
		OUT LPHANDLE lpWritePipe,
		IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
		IN DWORD nSize,
		DWORD dwReadMode,
		DWORD dwWriteMode,
        DWORD nDefaultTimeOut)
	{
        HANDLE readPipeHandle, writePipeHandle;
        DWORD dwError;
        char pipeNameBuffer[MAX_PATH];

        //
        // Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
        //

        if ((dwReadMode | dwWriteMode) & (~FILE_FLAG_OVERLAPPED)) {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        //
        //  Set the default timeout to 120 seconds
        //

        if (nSize == 0) {
            nSize = 4096;
        }

        sprintf_s(pipeNameBuffer,
            "\\\\.\\Pipe\\RemoteExeAnon.%08x.%08x",
            GetCurrentProcessId(),
            MyCreatePipeEx::PipeSerialNumber++
        );

        readPipeHandle = CreateNamedPipeA(
            pipeNameBuffer,
            PIPE_ACCESS_INBOUND | dwReadMode,
            PIPE_TYPE_BYTE | PIPE_WAIT,
            1,             // Number of pipes
            nSize,         // Out buffer size
            nSize,         // In buffer size
            nDefaultTimeOut,    // Timeout in ms
            lpPipeAttributes
        );

        if (!readPipeHandle) {
            return FALSE;
        }

        writePipeHandle = CreateFileA(
            pipeNameBuffer,
            GENERIC_WRITE,
            0,                         // No sharing
            lpPipeAttributes,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | dwWriteMode,
            NULL                       // Template file
        );

        if (INVALID_HANDLE_VALUE == writePipeHandle) {
            dwError = GetLastError();
            CloseHandle(readPipeHandle);
            SetLastError(dwError);
            return FALSE;
        }

        *lpReadPipe = readPipeHandle;
        *lpWritePipe = writePipeHandle;

        return TRUE;
	}
}
