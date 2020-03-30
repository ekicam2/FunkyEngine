#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define FUNKY_WINDOWS
#else
#error "Platform not supported."
#endif

#include "Core/Thread/IThread.h"

#ifdef FUNKY_WINDOWS
#include "Core/Platform/Windows/FunkyWindows.h"
#include "Core/Platform/Windows/WindowsIOSystem.h"
#include "Core/Platform/Windows/WindowsFileUtils.h"
#endif