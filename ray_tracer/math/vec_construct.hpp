#pragma once

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace math
{
	// vector 2
	inline vec2 to_vec2(const vec3& vec)
	{
		return vec2(vec.x, vec.y);
	}

	inline vec2 to_vec2(const vec4& vec)
	{
		return vec2(vec.x, vec.y);
	}

	// vector 3
	inline vec3 to_vec3(const vec3& vec)
	{
		return vec3(vec.x, vec.y, 0.f);
	}

	inline vec3 to_vec3(const vec2& vec, float z)
	{
		return vec3(vec.x, vec.y, z);
	}

	inline vec3 to_vec3(float x, const vec2& vec)
	{
		return vec3(x, vec.x, vec.y);
	}

	inline vec3 to_vec3(const vec4& vec)
	{
		return vec3(vec.x, vec.y, vec.z);
	}

	// vector 4
	inline vec4 to_vec4(const vec2& xy, const vec2& zw)
	{
		return vec4(xy.x, xy.y, zw.x, zw.y);
	}

	inline vec4 to_vec4(const vec2& xy, float z, float w)
	{
		return vec4(xy.x, xy.y, z, w);
	}

	inline vec4 to_vec4(float x, float y, const vec2& zw)
	{
		return vec4(x, y, zw.x, zw.y);
	}

	inline vec4 to_vec4(const vec3& xyz, float w)
	{
		return vec4(xyz.x, xyz.y, xyz.z, w);
	}

	inline vec4 to_vec4(float x, const vec3& yzw)
	{
		return vec4(x, yzw.x, yzw.y, yzw.z);
	}
}