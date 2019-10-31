#pragma once

#include "BasicTypes.h"

namespace Funky
{
	namespace Core
	{
		namespace IO
		{
			enum class Key : unsigned
			{
				A,
				B,
				C,
				D,
				E,
				F,
				G,
				H,
				I,
				J,
				K,
				L,
				M,
				N,
				O,
				P,
				Q,
				R,
				S,
				T,
				U,
				W,
				X,
				Y,
				Z,

				ARROW_UP,
				ARROW_DOWN,
				ARROW_LEFT,
				ARROW_RIGHT,

				TAB,
				SPACE,

				COUNT
			};

			class IIOSystem
			{
			public:
				/* Remember to always call Frame() at the beginning of overloaded Update()! */
				virtual void Update() = 0;

				FORCEINLINE bool IsKeyPressed(IO::Key Key) const { return Keys[(u32)Key].IsPressed; }
				FORCEINLINE bool WasKeyPressed(IO::Key Key) const { return Keys[(u32)Key].WasPressedLastFrame; }

			protected:
				void Frame();

				struct KeyState
				{
					u8 IsPressed : 1;
					u8 WasPressedLastFrame : 1;
				};

				KeyState Keys[(u32)Key::COUNT];
			};

		}
	}
}