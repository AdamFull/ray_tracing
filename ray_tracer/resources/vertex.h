#pragma once

struct FVertex
{
	FVertex() = default;
	FVertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv) : m_position(pos), m_normal(norm), m_texcoord(uv) {}

	glm::vec3 m_color{};
	glm::vec3 m_position{};
	glm::vec3 m_normal{};
	glm::vec2 m_texcoord{};
};