#pragma once

#include "LogMacros.h"

#include <assert.h>
#define WINDOWS_LEAN_AND_MEAN
#include "Windows.h"
#include <stdio.h>

#ifdef _DEBUG

//TODO(ekicam2): abstract strcpy and play with more templates for charx deduction
#if !UNICODE
#error "UNICODE switch has to be turned on, Properties -> General -> Character Set"
#endif

namespace 
{
	template <typename T>
	bool OnceOnly(T const &)
	{
		static bool returner = true;
		if (returner) { returner = 0; return true; }
		return false;
	}

	bool ShowAssertionBox(wchar_t const * const pCond, wchar_t const * const pFile, int iLine, wchar_t const * pMsg = nullptr)
	{
		wchar_t pRealMsg[512];
		wcscpy_s(pRealMsg, 512, TEXT("Assertion failed: "));
		wcscat_s(pRealMsg, 512, pCond);
		wcscat_s(pRealMsg, 512, TEXT("  \n"));
		wcscat_s(pRealMsg, 512, pMsg);
		wcscat_s(pRealMsg, 512, TEXT("  \n"));
		wcscat_s(pRealMsg, 512, pFile);
		wcscat_s(pRealMsg, TEXT(":"));
		wchar_t pRealLine[5];
		swprintf(pRealLine, 5, TEXT("%d"), iLine);
		wcscat_s(pRealMsg, pRealLine);

		int answear = MessageBox(0, pRealMsg, TEXT("Error occured: "), MB_ABORTRETRYIGNORE | MB_SYSTEMMODAL);
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

#define ASSERT_ONCE(condition, msg) if(!(condition) && OnceOnly([]{})) { if(ShowAssertionBox(msg, TEXTIFY(condition), TEXTIFY(__FILE__), __LINE__, msg)) __debugbreak(); }
#define ASSERT(condition, msg) if(!(condition)) { LOG_ERROR(msg); if(ShowAssertionBox(TEXTIFY(condition), TEXTIFY(__FILE__), __LINE__, msg)) __debugbreak(); }

#define CHECK_ONCE(condition) if(!(condition) && OnceOnly([]{})) { if(ShowAssertionBox(TEXTIFY(condition), TEXTIFY(__FILE__), __LINE__)) __debugbreak(); }
#define CHECK(condition) if(!(condition)) { if(ShowAssertionBox(TEXTIFY(condition), TEXTIFY(__FILE__), __LINE__)) __debugbreak(); }


#define DEATH_PATH(...) ASSERT(false, TEXT("death path reached"))
#define BREAK(...) (__noop, __debugbreak())

#define DEF_DEBUG_SCOPE_TIMER(TimerName) ScopeTimer<Timer::EResolution::Mills> CONCAT(ScopeTimer, __LINE__)(TimerName)
#define DEF_DEBUG_SCOPE_TIMER_MICRO(TimerName) ScopeTimer<Timer::EResolution::Micro> CONCAT(ScopeTimer, __LINE__)(TimerName)
#define DEF_DEBUG_SCOPE_TIMER_SEC(TimerName) ScopeTimer<Timer::EResolution::Sec> CONCAT(ScopeTimer, __LINE__)(TimerName)

#else

#define DEATH_PATH(...) _assume(0)
#define ASSERT_ONCE(...)
#define ASSERT(...)
#define CHECK_ONCE(...) 
#define CHECK(...)
#define BREAK(...) 
#define DEF_DEBUG_SCOPE_TIMER(...)
#define DEF_DEBUG_SCOPE_TIMER_MICRO(...)
#define DEF_DEBUG_SCOPE_TIMER_SEC(...)

#endif