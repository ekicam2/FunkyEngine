#pragma once

#include "LogMacros.h"

#include <assert.h>
#define WINDOWS_LEAN_AND_MEAN
#include "Windows.h"
#include <stdio.h>

#ifdef _DEBUG
//TODO(ekicam2): abstract strcpy and play with more templates for charx deduction

#define __STRINGIFY(x) TEXT(#x)
#define STRINGIFY(x) __STRINGIFY(x)
#define MACRO_TO_STRING(macro) STRINGIFY(macro)

namespace 
{
	template <typename T>
	bool OnceOnly(T const &)
	{
		static bool returner = true;
		if (returner) { returner = 0; return true; }
		return false;
	}

	[[maybe_unused]] bool ShowAssertionBox(wchar_t const * const pCond, wchar_t const * const pFile, int iLine, wchar_t const * pMsg = nullptr)
	{
		wchar_t pRealMsg[512];
		wcscpy_s(pRealMsg, L"Assertion failed: ");
		wcscat_s(pRealMsg, pCond);
		wcscat_s(pRealMsg, L"  \n");
		if (pMsg != nullptr)
		{
			wcscat_s(pRealMsg, pMsg);
			wcscat_s(pRealMsg, L"  \n");
		}
		wcscat_s(pRealMsg, pFile);
		wcscat_s(pRealMsg, L":");
		wchar_t pRealLine[5];
		swprintf(pRealLine, 5, L"%d", iLine);
		wcscat_s(pRealMsg, pRealLine);

		int answear = MessageBoxW(0, pRealMsg, L"Error occured: ", MB_ABORTRETRYIGNORE | MB_SYSTEMMODAL);
		switch (answear)
		{
			case IDABORT:
				exit(-1);
				break;

			case IDRETRY:
				return true;
				break;

			case IDIGNORE:
			default:
				return false;
				break;
		}
	}

	[[maybe_unused]] bool ShowAssertionBox(char const* const pCond, char const* const pFile, int iLine, char const* pMsg = nullptr)
	{
		char pRealMsg[512];
		
		strcpy_s(pRealMsg, "Assertion failed: ");
		strcat_s(pRealMsg, pCond);
		strcat_s(pRealMsg, "  \n");
		if (pMsg != nullptr)
		{
			strcat_s(pRealMsg, pMsg);
			strcat_s(pRealMsg, "  \n");
		}
		strcat_s(pRealMsg, pFile);
		strcat_s(pRealMsg, ":");
		char pRealLine[5];
		sprintf_s(pRealLine, "%d", iLine);
		strcat_s(pRealMsg, pRealLine);

		int answear = MessageBoxA(0, pRealMsg, "Error occured: ", MB_ABORTRETRYIGNORE | MB_SYSTEMMODAL);
		switch (answear)
		{
		case IDABORT:
			exit(-1);
			break;

		case IDRETRY:
			return true;
			break;

		case IDIGNORE:
		default:
			return false;
			break;
		}
	}
}

#define BREAK(...) (__noop, __debugbreak())

#define ASSERT_ONCE(condition, msg) if(!(condition) && OnceOnly([]{})) { if(ShowAssertionBox(msg, MACRO_TO_STRING(condition), MACRO_TO_STRING(__FILE__), __LINE__, msg)) BREAK(); }
#define ASSERT(condition, msg) if(!(condition)) { LOG_ERROR(msg); if(ShowAssertionBox(MACRO_TO_STRING(condition), MACRO_TO_STRING(__FILE__), __LINE__, msg)) BREAK(); }

#define CHECK_ONCE(condition) if(!(condition) && OnceOnly([]{})) { if(ShowAssertionBox(MACRO_TO_STRING(condition), MACRO_TO_STRING(__FILE__), __LINE__)) BREAK(); }
#define CHECK(condition) if(!(condition)) { if(ShowAssertionBox(MACRO_TO_STRING(condition), MACRO_TO_STRING(__FILE__), __LINE__)) BREAK(); }

#define DEAD_PATH(...) ASSERT(false, TEXT("death path reached"))

#else

#define DEAD_PATH(...) __assume(0)
#define ASSERT_ONCE(...)
#define ASSERT(...)
#define CHECK_ONCE(...) 
#define CHECK(...)
#define BREAK(...) 
#define DEBUG_SCOPE_TIMER(...)
#define DEF_DEBUG_SCOPE_TIMER_MICRO(...)
#define DEF_DEBUG_SCOPE_TIMER_SEC(...)

#endif