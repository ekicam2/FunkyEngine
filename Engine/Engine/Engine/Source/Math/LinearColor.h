#pragma once

#include <BasicTypes.h>

namespace Math
{
	struct LinearColor
	{
		f32 R, G, B, A;

		LinearColor()
			: LinearColor(0.0f, 0.0f, 0.0f, 0.0f)
		{

		}

		LinearColor(f32 const & R, f32 const & G, f32 const & B, f32 const & A)
			: R(R)
			, G(G)
			, B(B)
			, A(A)
		{

		}

		LinearColor& operator=(LinearColor const & New)
		{
			R = New.R;
			G = New.G;
			B = New.B;
			A = New.A;

			return *this;
		}

		LinearColor& operator*(f32 Value)
		{
			R *= Value;
			G *= Value;
			B *= Value;
			A *= Value;

			return *this;
		}
	};
}
