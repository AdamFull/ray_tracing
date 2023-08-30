#pragma once

struct FTransformComponent
{
	// local
	glm::vec3 m_position{ 0.f };
	glm::quat m_rotation{ 1.f, 0.f, 0.f, 0.f };
	glm::vec3 m_scale{ 1.f };

	// global
	glm::vec3 m_position_g{ 0.f };
	glm::quat m_rotation_g{ 1.f, 0.f, 0.f, 0.f };
	glm::vec3 m_scale_g{ 1.f };

	glm::mat4 m_model{ 1.f };
	glm::mat4 m_normal{ 1.f };
	glm::mat4 m_modelOld{ 1.f };
	glm::mat4 m_matrix{ 1.f };
};