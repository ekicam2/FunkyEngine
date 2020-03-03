#pragma once

#include "BasicTypes.h"

namespace Funky::Rendering
{
	class Event
	{
	public:
		Event()			= delete;
		Event(Event&)	= delete;
		Event(Event&&)	= delete;

		Event(const charx* Name);
		virtual ~Event() {}
	};
}

#define GPU_SCOPE_EVENT(Name) Event(Name);