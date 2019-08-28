#include "Timer.h"

FORCEINLINE LARGE_INTEGER _helperfunc()
{
	LARGE_INTEGER Temp;
	::QueryPerformanceFrequency(&Temp);
	return Temp;
}

const LARGE_INTEGER Timer::m_Frequency = _helperfunc();
