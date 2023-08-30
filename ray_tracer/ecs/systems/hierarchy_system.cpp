#include "hierarchy_system.h"

#include "engine.h"

#include "ecs/components/transform_component.h"
#include "ecs/components/hierarchy_component.h"

#include <glm/gtx/matrix_decompose.hpp>

void CHierarchySystem::create(CRayEngine* engine)
{
	update(engine);
}

void CHierarchySystem::update(CRayEngine* engine)
{
	auto& scene = engine->get_scene();
	auto& registry = scene->get_registry();
	auto& root = scene->get_root();

	build_hierarchy(registry, root);
}

void CHierarchySystem::build_hierarchy(entt::registry& registry, const entt::entity& node)
{
	auto& transform = registry.get<FTransformComponent>(node);
	auto& hierarchy = registry.get<FHierarchyComponent>(node);

	transform.m_modelOld = transform.m_model;
	initialize_matrix(&transform);

	if (registry.valid(hierarchy.parent))
	{
		auto parent_transform = registry.get<FTransformComponent>(hierarchy.parent);
		transform.m_model = parent_transform.m_model * transform.m_model;
	}

	glm::vec3 skew;
	glm::vec4 persp;
	glm::decompose(transform.m_model, transform.m_scale_g, transform.m_rotation_g, transform.m_position_g, skew, persp);

	for (auto& child : hierarchy.children)
		build_hierarchy(registry, child);

	transform.m_normal = glm::transpose(glm::inverse(transform.m_model));
}

void CHierarchySystem::initialize_matrix(FTransformComponent* transform)
{
	transform->m_model = glm::translate(glm::mat4(1.0f), transform->m_position) * glm::mat4(transform->m_rotation) * glm::scale(glm::mat4(1.0f), transform->m_scale) * transform->m_matrix;
}