#pragma once

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

namespace
{
	template<typename T>
	void IDELOG(T msg)
	{
#if UNICODE
		OutputDebugString(std::to_wstring(msg).c_str());
#else 
		OutputDebugString(std::to_string(msg).c_str());
#endif
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

#if UNICODE
#define STDLOG std::wclog
#define STDERR std::wcerr

#else
#define STDLOG std::clog
#define STDERR std::cerr
#endif


#ifdef PREF_IDE_LOG

#define __INTERNAL_LOG(debug_msg) IDELOG(debug_msg);
#define __INTERNAL_ERR(debug_msg) IDELOG(debug_msg);

#elif defined(PREF_CONSOLE_LOG)

#define __INTERNAL_LOG(debug_msg) STDLOG << debug_msg;	
#define __INTERNAL_ERR(debug_msg) STDERR << debug_msg;	

#elif defined(LOG_EVERYWHERE)

#define __INTERNAL_LOG(debug_msg) STDLOG << debug_msg; IDELOG(debug_msg);
#define __INTERNAL_ERR(debug_msg) STDERR << debug_msg; IDELOG(debug_msg);

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

#define LOG_FUNKY(...)	LogF(__VA_ARGS__)
#define LOG_ERROR_FUNKY(...)	ErrF(__FILE__, TEXT(":"), __LINE__, TEXT("\t"), __VA_ARGS__)



namespace Funky
{
	enum class ELogType
	{
		Info,
		Warning,
		Error
	};

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
			case ELogType::Info:	return "[INFO]		| ";
			case ELogType::Warning: return "[WARNING]	| ";
			case ELogType::Error:	return "Error		| ";
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


#else

#define LOG_FUNKY(...)	

#define LOG_ERROR(msg) 
#define LOG_ERROR_FUNKY(...) 

#endif