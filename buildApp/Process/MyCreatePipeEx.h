#pragma once

#include <atomic>
#include <Windows.h>

namespace Process
{
	// functor base on https://stackoverflow.com/questions/60645/overlapped-i-o-on-anonymous-pipe
	class MyCreatePipeEx
	{
	public:

        /*++
        Routine Description:
            The CreatePipeEx API is used to create an anonymous pipe I/O device.
            Unlike CreatePipe FILE_FLAG_OVERLAPPED may be specified for one or
            both handles.
            Two handles to the device are created.  One handle is opened for
            reading and the other is opened for writing.  These handles may be
            used in subsequent calls to ReadFile and WriteFile to transmit data
            through the pipe.
        Arguments:
            lpReadPipe - Returns a handle to the read side of the pipe.  Data
                may be read from the pipe by specifying this handle value in a
                subsequent call to ReadFile.
            lpWritePipe - Returns a handle to the write side of the pipe.  Data
                may be written to the pipe by specifying this handle value in a
                subsequent call to WriteFile.
            lpPipeAttributes - An optional parameter that may be used to specify
                the attributes of the new pipe.  If the parameter is not
                specified, then the pipe is created without a security
                descriptor, and the resulting handles are not inherited on
                process creation.  Otherwise, the optional security attributes
                are used on the pipe, and the inherit handles flag effects both
                pipe handles.
            nSize - Supplies the requested buffer size for the pipe.  This is
                only a suggestion and is used by the operating system to
                calculate an appropriate buffering mechanism.  A value of zero
                indicates that the system is to choose the default buffering
                scheme.
        Return Value:
            TRUE - The operation was successful.
            FALSE/NULL - The operation failed. Extended error status is available
                using GetLastError.
        --*/
		BOOL APIENTRY operator()(
			OUT LPHANDLE lpReadPipe,
			OUT LPHANDLE lpWritePipe,
			IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
			IN DWORD nSize,
			DWORD dwReadMode,
			DWORD dwWriteMode,
            DWORD nDefaultTimeOut = 120 * 1000 // Timeout in ms
            );

	private:
		static std::atomic<uint32_t> PipeSerialNumber;
	};
}
