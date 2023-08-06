#pragma once

struct FVertex
{
	FVertex() = default;
	FVertex(const math::vec3& pos, const math::vec3& norm, const math::vec2& uv) : position(pos), normal(norm), texcoord(uv) {}

	math::vec3 color{};
	math::vec3 position{};
	math::vec3 normal{};
	math::vec2 texcoord{};
};