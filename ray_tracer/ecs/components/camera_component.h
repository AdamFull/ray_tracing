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

	bool m_bWasMoved{ true };
};