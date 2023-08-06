#pragma once

#include "vec_construct.hpp"

#include "mat2.hpp"
#include "mat3.hpp"
#include "mat4.hpp"

namespace math
{
	mat3 to_mat3(const mat4& mat)
	{
		return mat3(to_vec3(mat[0ull]), to_vec3(mat[1ull]), to_vec3(mat[2ull]));
	}
}