#pragma once

#ifdef _DEBUG
#define FORCEINLINE_DEBUGGABLE
#else
#define FORCEINLINE_DEBUGGABLE __forceinline
#endif

#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

#define FORCEINLINE __forceinline

#define _CONCAT_(First, Second) First ## Second
#define CONCAT(First, Second) _CONCAT_(First, Second)

#if _UNICODE
	#define _TEXTIFY(Quote) CONCAT(L, #Quote)
#else
	#define _TEXTIFY(Quote) #Quote
#endif

#define TEXTIFY(Quote) _TEXTIFY(Quote)
