#pragma once

struct FSpotLightComponent
{
	glm::vec3 m_color{ 1.f };
	glm::vec3 m_target{ 0.f, 0.f, 1.f };
	float m_intencity{ 1.f };
	float m_innerAngle{ 0.f };
	float m_outerAngle{ 0.78f };
	bool m_toTarget{ false };
};