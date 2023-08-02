#pragma once

#include "util.h"

enum class ECameraType
{
	eOrthographic,
	ePerspective
};

struct FCameraComponent
{
	ECameraType m_type{ ECameraType::ePerspective };

	float m_fov{ 45.f };
	float m_near{ 0.1f };
	float m_far{ 100.f };

	float m_xmag{ 0.f };
	float m_ymag{ 0.f };

	glm::mat4 m_view{ 1.f }, m_invView{ 1.f }, m_projection{ 1.f }, m_invProjection{ 1.f };

	glm::vec3 m_up{ 0.f }, m_forward{ 0.f }, m_right{ 0.f };

	glm::uvec2 m_viewportExtent{ 0u };

	std::vector<glm::vec3> m_vRayDirections{};

	glm::vec3 get_ray(uint32_t x, uint32_t y)
	{
		glm::vec2 texcoord = { (static_cast<float>(x) + random<float>()) / static_cast<float>(m_viewportExtent.x), (static_cast<float>(y) + random<float>()) / static_cast<float>(m_viewportExtent.y) };
		texcoord = texcoord * 2.f - 1.f;

		glm::vec4 target = m_invProjection * glm::vec4(texcoord.x, texcoord.y, 1.f, 1.f);
		return glm::vec3(m_invView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0.f));
	}

	bool m_bWasMoved{ true };
};