#include "WindowsFileUtils.h"

#include "FunkyWindows.h"
#include "BasicTypes.h"
#include "LogMacros.h"

#include "Core/Memory/UniquePtr.h"


Str Funky::Platform::ReadFile(Str const& FilePath)
{
	HANDLE hFile;

	hFile = ::CreateFileA(FilePath.GetBuffer(),               // file to open
		GENERIC_READ,          // open for reading
		0,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_READONLY, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		Core::Memory::UniquePtr<char[]> MsgBuffer(new char[1024]);
		sprintf_s(MsgBuffer.Get(), 1024, "Unable to open file \"%s\" for read.", FilePath.GetBuffer());
		LOG_ERROR(MsgBuffer.Get());
		LOG_ERROR(GetLastError());
		return "";
	}


	constexpr size BufferSize = 1 << 13;
	// reserve 4KB for a file
	Core::Memory::UniquePtr<char[]> Buffer(new char[BufferSize]);

	DWORD Read;

	BOOL Succeed = ::ReadFile(
		hFile,
		Buffer,
		BufferSize - 1,
		&Read,
		nullptr
	);

	if (!Succeed)
	{
		Core::Memory::UniquePtr<char[]> MsgBuffer(new char[1024]);
		sprintf_s(MsgBuffer.Get(), 1024, "Unable to read from file.\n GetLastError=%08x", GetLastError());
		LOG_ERROR(MsgBuffer.Get());

		CloseHandle(hFile);
		return "";
	}

	//((char*)Buffer)

	CloseHandle(hFile);
	return (Read == 0 ? "" : Str(Buffer, Read));
}

