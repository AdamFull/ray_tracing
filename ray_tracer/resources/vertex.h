#pragma once

struct FVertex
{
	FVertex() = default;
	FVertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv) : position(pos), normal(norm), texcoord(uv) {}

	glm::vec3 color{};
	glm::vec3 position{};
	glm::vec3 normal{};
	glm::vec2 texcoord{};
};