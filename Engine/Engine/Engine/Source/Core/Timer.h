#pragma once

#include "BasicTypes.h"
#include "LogMacros.h"

#define WINDOWS_LEAN_AND_MEAN
#include "windows.h"

#undef min
#undef max

class Timer
{
public:
	enum class EResolution
	{
		Micro,
		Mills,
		Sec
	};

	void Begin() {
		::QueryPerformanceCounter(&m_Timers[0]);
		m_bFinished = false;
	}

	double Reset(EResolution Resolution) {
		End();
		double Ret;
		switch (Resolution)
		{
		case Timer::EResolution::Micro:
			Ret = GetInMicro();
			break;
		case Timer::EResolution::Mills:
			Ret = GetInMills();
			break;
		case Timer::EResolution::Sec:
			Ret = GetInSec();
			break;
		default:
			Ret = 0.0;
			break;
		}
		Begin();

		return Ret;
	}

	void End() {
		::QueryPerformanceCounter(&m_Timers[1]);
		m_bFinished = true;
	}

	inline double GetInSec() { return (m_bFinished ? ((double)(m_Timers[1].QuadPart - m_Timers[0].QuadPart) / Timer::m_Frequency.QuadPart) : 0); }
	inline double GetInMills() { return (m_bFinished ? ((double)(m_Timers[1].QuadPart - m_Timers[0].QuadPart) / Timer::m_Frequency.QuadPart * 1000.) : 0); }
	inline double GetInMicro() { return (m_bFinished ? ((double)(m_Timers[1].QuadPart - m_Timers[0].QuadPart) / Timer::m_Frequency.QuadPart * 1000000.) : 0.); }

private:
	static const LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_Timers[2];
	bool m_bFinished = false;
};

#ifdef _DEBUG

template<typename T, Timer::EResolution Resolution>
class DebugLogScopeTimer final : private Timer
{
	T const* const m_pTimerName;
public:

	DebugLogScopeTimer(T const* const pTimerName) : m_pTimerName(pTimerName) { Begin(); }
	~DebugLogScopeTimer()
	{
		End();
		if constexpr (Resolution == Timer::EResolution::Micro)
		{
			LOG(m_pTimerName, TEXT(" elapsed time: "), GetInMicro(), TEXT("?s."));
		}
		else if constexpr (Resolution == Timer::EResolution::Mills)
		{
			LOG(m_pTimerName, TEXT(" elapsed time: "), GetInMills(), TEXT("ms."));
		}
		else if constexpr (Resolution == Timer::EResolution::Sec)
		{
			LOG(m_pTimerName, TEXT(" elapsed time: "), GetInSec(), TEXT("s."));
		}
	}
};

template <Timer::EResolution Resolution>
using ScopeTimer = DebugLogScopeTimer<charx, Resolution>;

#define DEBUG_SCOPE_TIMER(TimerName) ScopeTimer<Timer::EResolution::Mills> CONCAT(ScopeTimer, __LINE__)(TimerName)
#define DEF_DEBUG_SCOPE_TIMER_MICRO(TimerName) ScopeTimer<Timer::EResolution::Micro> CONCAT(ScopeTimer, __LINE__)(TimerName)
#define DEF_DEBUG_SCOPE_TIMER_SEC(TimerName) ScopeTimer<Timer::EResolution::Sec> CONCAT(ScopeTimer, __LINE__)(TimerName)

#endif