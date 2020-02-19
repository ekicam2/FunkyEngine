#pragma once

#define ENDABLE_LOG
#define PREF_CONSOLE_LOG

#include "BasicTypes.h"

#include <iostream>
#include <sstream>
#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN
#endif
#include "windows.h"
//TODO: log to file or something

/*
#define PREF_IDE_LOG
#define PREF_CONSOLE_LOG
*/

#ifdef ENDABLE_LOG

#if (!defined(LOG_EVERYWHERE) && (!defined(PREF_IDE_LOG) && !defined(PREF_CONSOLE_LOG)))
#error "PREF_IDE_LOG or PREF_CONSOLE_LOG should be defined before includinf LogMacros"
#elif (defined(PREF_IDE_LOG) && defined(PREF_CONSOLE_LOG))
#error "PREF_IDE_LOG and PREF_CONSOLE_LOG should not be defined simultaneously"
#endif

#define INIT_LOG(...) 								\
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);	\
DWORD consoleMode;									\
GetConsoleMode(console, &consoleMode);				\
consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;	\
SetConsoleMode(console, consoleMode);					


enum class ELogType
{
	Info,
	Warning,
	Error
};

namespace std
{
	using str = std::basic_string<charx, std::char_traits<charx>, std::allocator<charx>>;
}

#if _UNICODE
	#define sprintf_s swprintf_s
#else
	#define sprintf_s sprintf_s
#endif

namespace
{
	template<typename T>
	void IDELOG(T msg)
	{
		//auto m2 = std::to_string(2).c_str();

		charx m[1024];
		sprintf_s(m, TEXT("%s"), msg);
		OutputDebugString(m);
	}

	template<>
	void IDELOG<float>(float msg)
	{
		charx m[1024];
		sprintf_s(m, TEXT("%f"), msg);
		OutputDebugString(m);
	}

	template<>
	void IDELOG<long>(long msg)
	{
		charx m[1024];
		sprintf_s(m, TEXT("%ld"), msg);
		OutputDebugString(m);
	}

	template<>
	void IDELOG<int>(int msg)
	{
		charx m[1024];
		sprintf_s(m, TEXT("%d"), msg);
		OutputDebugString(m);
	}


	template<>
	void IDELOG<std::str>(std::str msg)
	{
		OutputDebugString(msg.c_str());
	}

	template<>
	void IDELOG<char const *>(char const * msg)
	{
		OutputDebugStringA(msg);
	}

	template<>
	void IDELOG<wchar_t const *>(wchar_t const * msg)
	{
		OutputDebugStringW(msg);
	}
}

namespace
{
	template<typename T>
	void CONSOLELOG(T msg)
	{
		std::clog << msg;
	}

	template<>
	void CONSOLELOG<std::string const&>(std::string const& msg)
	{
		std::clog << msg.c_str();
	}

	template<>
	void CONSOLELOG<std::wstring const&>(std::wstring const& msg)
	{
		std::wclog << msg;
	}

	template<>
	void CONSOLELOG<char const*>(char const* msg)
	{
		std::clog << msg;
	}

	template<>
	void CONSOLELOG<char*>(char* msg)
	{
		std::clog << msg;
	}

	template<>
	void CONSOLELOG<wchar_t const*>(wchar_t const* msg)
	{
		std::wclog << msg;
	}

	template<>
	void CONSOLELOG<wchar_t*>(wchar_t* msg)
	{
		std::wclog << msg;
	}
}

namespace
{
	template<typename T>
	void CONSOLEERROR(T msg)
	{
		std::cerr << msg;
	}

	template<>
	void CONSOLEERROR<std::string const&>(std::string const& msg)
	{
		std::cerr << msg.c_str();
	}

	template<>
	void CONSOLEERROR<std::wstring const&>(std::wstring const& msg)
	{
		std::wcerr << msg;
	}

	template<>
	void CONSOLEERROR<char const*>(char const* msg)
	{
		std::cerr << msg;
	}

	template<>
	void CONSOLEERROR<wchar_t const*>(wchar_t const* msg)
	{
		std::wcerr << msg;
	}
}

#ifdef PREF_IDE_LOG

#define __INTERNAL_LOG(debug_msg) IDELOG(debug_msg);
#define __INTERNAL_ERR(debug_msg) IDELOG(debug_msg);

#elif defined(PREF_CONSOLE_LOG)

#define __INTERNAL_LOG(debug_msg) CONSOLELOG(debug_msg);	
#define __INTERNAL_ERR(debug_msg) CONSOLEERROR(debug_msg);	

#elif defined(LOG_EVERYWHERE)

#define __INTERNAL_LOG(debug_msg) CONSOLELOG(debug_msg); IDELOG(debug_msg);
#define __INTERNAL_ERR(debug_msg) CONSOLEERROR(debug_msg); IDELOG(debug_msg);

#endif

namespace
{
	template<typename... Ts>
	struct _LogF;

	template<typename T, typename... Ts>
	struct _LogF<T, Ts...>
	{
		static void log(T const & Param, Ts... Rest)
		{
			__INTERNAL_LOG(Param);
			_LogF<Ts...>::log(Rest...);
		}
	};

	template<typename T>
	struct _LogF<T>
	{
		static void log(T const & Param)
		{
			__INTERNAL_LOG(Param);
			__INTERNAL_LOG(TEXT("\n"));
		}
	};

	template<typename... Ts>
	struct _ErrF;

	template<typename T, typename... Ts>
	struct _ErrF<T, Ts...>
	{
		static void log(T const & Param, Ts... Rest)
		{
			__INTERNAL_ERR(Param);
			_ErrF<Ts...>::log(Rest...);
		}
	};

	template<typename T>
	struct _ErrF<T>
	{
		static void log(T const & Param)
		{
			__INTERNAL_ERR(Param);
			__INTERNAL_ERR(TEXT("\n"));
		}
	};
}

template<typename... Ts>
__forceinline void LogF(Ts... Params)
{
	_LogF<Ts...>::log(Params...);
}

template<typename... Ts>
__forceinline void ErrF(Ts... Params)
{
	_ErrF<Ts...>::log(Params...);
}

namespace
{
	__forceinline auto LogTime()
	{
		SYSTEMTIME Time;
		GetLocalTime(&Time);

		std::stringstream s;

		s << std::to_string(Time.wYear)
			<< "_"
			<< std::to_string(Time.wMonth)
			<< "_"
			<< std::to_string(Time.wDay)
			<< "("
			<< std::to_string(Time.wDayOfWeek)
			<< ") "
			<< std::to_string(Time.wHour)
			<< ":"
			<< std::to_string(Time.wMinute)
			<< ":"
			<< std::to_string(Time.wSecond)
			<< "."
			<< std::to_string(Time.wMilliseconds);

		return s.str();
	};

	constexpr auto LogTypeToHeader(ELogType LogType)
	{
		switch (LogType)
		{
#ifdef PREF_CONSOLE_LOG
		case ELogType::Info:	return "[\x1B[32mINFO\033[0m] | ";
		case ELogType::Warning: return "[\x1B[34mWARNING\033[0m] | ";
		case ELogType::Error:	return "[\x1b[1;31mError\033[0m] | ";
#else
		case ELogType::Info:	return "[INFO] | ";
		case ELogType::Warning: return "[WARNING] | ";
		case ELogType::Error:	return "[Error] | ";
#endif
		default: return "";
		}
	};

	template <ELogType LogType, typename... Ts>
	__forceinline void Log(Ts... Params)
	{
		std::stringstream ss;
		ss << LogTime()
			<< " "
			<< LogTypeToHeader(LogType);

		_LogF<char const*, Ts...>::log(ss.str().c_str(), Params...);
	}
}

#define LOG(...)	Log<ELogType::Info>(__VA_ARGS__)
//#define LOG_ERROR(msg)  ErrF(__FILE__, TEXT(":"), __LINE__, TEXT("\t"), msg)
#define LOG_WARNING(...)	Log<ELogType::Warning>(__VA_ARGS__)
#define LOG_ERROR(...)	Log<ELogType::Error>(__FILE__, TEXT(":"), __LINE__, TEXT("\t"), __VA_ARGS__)

#undef sprintf_s

#else

#define LOG(...)	
#define LOG_ERROR(msg) 
#define ERROR(...)
}
#endif
