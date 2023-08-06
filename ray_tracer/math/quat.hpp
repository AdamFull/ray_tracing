#pragma once

#include "math_template.hpp"

namespace math
{
	struct quat
	{
		quat() : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 1.f } {}
		quat(float _w, float _x, float _y, float _z) : w{ _w }, x{ _x }, y{ _y }, z{ _z } {}

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			__m128 vec128;
		};
	};

	inline quat make_quat(const float* data)
	{
		return quat();

	}

	inline quat make_quat(const double* data)
	{
		return quat();
	}
}