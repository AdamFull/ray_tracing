#pragma once

#include "vertex.h"

struct mesh_template
{
	std::vector<FVertex> vertices;
	std::vector<uint32_t> indices;
};

struct cube_template : mesh_template
{
	cube_template()
	{
		vertices =
		{
			//-----|---------position----------|------------normal-----------|-------texcoord-------|
			FVertex(math::vec3(-1.f, -1.f,  1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 1.f,  1.f)),
			FVertex(math::vec3(-1.f, -1.f, -1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 1.f,  0.f)),
			FVertex(math::vec3( 1.f, -1.f, -1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 0.f,  0.f)),
			FVertex(math::vec3( 1.f, -1.f,  1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 0.f,  1.f)),
			FVertex(math::vec3(-1.f,  1.f,  1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 0.f,  1.f)),
			FVertex(math::vec3( 1.f,  1.f,  1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 1.f,  1.f)),
			FVertex(math::vec3( 1.f,  1.f, -1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 1.f,  0.f)),
			FVertex(math::vec3(-1.f,  1.f, -1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 0.f,  0.f)),
			FVertex(math::vec3(-1.f, -1.f,  1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 0.f,  1.f)),
			FVertex(math::vec3( 1.f, -1.f,  1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 1.f,  1.f)),
			FVertex(math::vec3( 1.f,  1.f,  1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 1.f,  0.f)),
			FVertex(math::vec3(-1.f,  1.f,  1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 0.f,  0.f)),
			FVertex(math::vec3( 1.f, -1.f,  1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 0.f,  1.f)),
			FVertex(math::vec3( 1.f, -1.f, -1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 1.f,  1.f)),
			FVertex(math::vec3( 1.f,  1.f, -1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 1.f,  0.f)),
			FVertex(math::vec3( 1.f,  1.f,  1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 0.f,  0.f)),
			FVertex(math::vec3( 1.f, -1.f, -1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 0.f,  1.f)),
			FVertex(math::vec3(-1.f, -1.f, -1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 1.f,  1.f)),
			FVertex(math::vec3(-1.f,  1.f, -1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 1.f,  0.f)),
			FVertex(math::vec3( 1.f,  1.f, -1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 0.f,  0.f)),
			FVertex(math::vec3(-1.f, -1.f, -1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 0.f,  1.f)),
			FVertex(math::vec3(-1.f, -1.f,  1.f), math::vec3( 0.f, -1.f,  0.f), math::vec2( 1.f,  1.f)),
			FVertex(math::vec3(-1.f,  1.f,  1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 1.f,  0.f)),
			FVertex(math::vec3(-1.f,  1.f, -1.f), math::vec3( 0.f,  1.f,  0.f), math::vec2( 0.f,  0.f)),

		};

		indices =
		{
			0u,  1u,  2u,  2u,  3u,  0u,
			4u,  5u,  6u,  6u,  7u,  4u,
			8u,  9u,  10u, 10u, 11u, 8u,
			12u, 13u, 14u, 14u, 15u, 12u,
			16u, 17u, 18u, 18u, 19u, 16u,
			20u, 21u, 22u, 22u, 23u, 20u
		};
	}
};

struct quad_template : mesh_template
{
	quad_template()
	{
		vertices =
		{
			//------|---------position----------|-----------normal----------|-------texcoord-------|
			FVertex(math::vec3(-1.f,  1.f,  0.f), math::vec3( 0.f, 1.f,  0.f), math::vec2( 0.f,  0.f)),
			FVertex(math::vec3( 1.f,  1.f,  0.f), math::vec3( 0.f, 1.f,  0.f), math::vec2( 1.f,  0.f)),
			FVertex(math::vec3(-1.f, -1.f,  0.f), math::vec3( 0.f, 1.f,  0.f), math::vec2( 0.f,  1.f)),
			FVertex(math::vec3( 1.f, -1.f,  0.f), math::vec3( 0.f, 1.f,  0.f), math::vec2( 1.f,  1.f))
		};

		indices = { 0u, 1u, 3u, 0u, 3u, 2u };
	}
};

class CVertexBuffer
{
public:
	void add_vertices(const std::vector<FVertex>& vertices);
	void add_indices(const std::vector<uint32_t>& indices);

	void add_mesh_data(std::unique_ptr<mesh_template>&& mesh_template);
	void add_mesh_data(const std::vector<FVertex>& vertices, const std::vector<uint32_t>& indices);

	const FVertex& get_vertex(size_t index);
	uint32_t get_index(size_t index);

	size_t get_last_vertex() const;
	size_t get_last_index() const;

	void clear();
private:
	std::vector<FVertex> m_vertices{};
	std::vector<uint32_t> m_indices{};
};