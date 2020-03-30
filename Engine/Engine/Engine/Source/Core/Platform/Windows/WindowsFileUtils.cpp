#include "WindowsFileUtils.h"

#include "FunkyWindows.h"
#include "BasicTypes.h"
#include "LogMacros.h"

namespace 
{
	template <size Size>
	struct BufferHelper final
	{
		BufferHelper()
		{
			pBuffer = new char[Size];
		}
		~BufferHelper()
		{
			delete[] pBuffer;
		}

		char* operator* ()
		{
			return pBuffer;
		}

		operator char* ()
		{
			return pBuffer;
		}

		char* pBuffer = nullptr;
	};
}

str Funky::Platform::ReadFile(str const& FilePath)
{
	HANDLE hFile;

	hFile = ::CreateFileA(FilePath.c_str(),               // file to open
		GENERIC_READ,          // open for reading
		0,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_READONLY, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		BufferHelper<1 << 12> Buffer2;
		sprintf_s(*Buffer2, 1024, "unable to open file \"%s\" for read.", FilePath.c_str());
		LOG_ERROR(*Buffer2);
		LOG_ERROR(GetLastError());
		return "";
	}


	constexpr size BufferSize = 1 << 13;
	// reserve 4KB for a file
	BufferHelper<BufferSize> Buffer;
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
		LOG_ERROR(TEXT("ReadFile"));
		BufferHelper<1 << 12> Buffer2;
		sprintf_s(Buffer2, 1024, "Unable to read from file.\n GetLastError=%08x", GetLastError());
		LOG_ERROR(*Buffer2);

		CloseHandle(hFile);
		return "";
	}

	//((char*)Buffer)

	CloseHandle(hFile);
	return Read == 0 ? "" : str(Buffer, Read);
}

