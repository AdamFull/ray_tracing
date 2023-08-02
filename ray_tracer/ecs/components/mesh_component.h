#pragma once

#include "resources/vertex_buffer.h"

struct FPrimitiveTriangle
{
	const FVertex& v0;
	const FVertex& v1;
	const FVertex& v2;
};

struct FMeshPrimitive
{
	std::vector<FPrimitiveTriangle> m_triangles{};

	size_t m_begin_index{ 0ull }, m_index_count{ 0ull };
	size_t m_begin_vertex{ 0ull }, m_vertex_count{ 0ull };

	size_t m_material_id{ invalid_index };

	struct FPrimitiveDimensions
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(-min);
		glm::vec3 size;
		glm::vec3 center;
		float radius;
	} dimensions;

	void set_dimensions(const glm::vec3& min, const glm::vec3& max);
};

struct FMeshComponent
{
	size_t vbo_id{ invalid_index };
	std::vector<FMeshPrimitive> primitives;
};