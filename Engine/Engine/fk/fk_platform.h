#pragma once

#include "fk_type_defs.h"
#include "fk_memory.h"

// Platform deduction

#ifdef _WIN64
	#define FK_WINDOWS 1
#else
	#define FK_UNKNOWN 0
#endif


#define FK_MAX_FILE_PATH 1024

namespace fk
{
	charx* getExecPath();
}

#ifdef FK_IMPLEMENTATION


#if FK_WINDOWS

	#if FK_WINDOWS
		#ifndef WINDOWS_LEAN_AND_MEAN
			#define WINDOWS_LEAN_AND_MEAN
		#endif
		#include <windows.h>
	#endif

charx* __getExecPath()
{
	TCHAR szFileName[MAX_PATH + 1];
	DWORD size = GetModuleFileName(NULL, szFileName, MAX_PATH + 1);
	if (size > 0L)
	{
		charx* returner = new charx[size + 1];
		fk::memCpy(szFileName, returner, (size + 1) * sizeof(charx));

		return returner;
	}

	return nullptr;
}
#endif

charx* fk::getExecPath()
{
#if FK_WINDOWS
	return __getExecPath();
#else
	#error "charx fk::getExecPath() not implemented"
#endif
}

#endif // FK_IMPLEMENTATION
