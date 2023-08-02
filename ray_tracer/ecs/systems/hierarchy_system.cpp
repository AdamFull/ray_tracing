#include "hierarchy_system.h"

#include "engine.h"

#include "ecs/components/transform_component.h"
#include "ecs/components/hierarchy_component.h"

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

	for (auto& child : hierarchy.children)
		build_hierarchy(registry, child);

	transform.m_normal = glm::transpose(glm::inverse(transform.m_model));
}

void CHierarchySystem::initialize_matrix(FTransformComponent* transform)
{
	transform->m_model = glm::translate(glm::mat4(1.0f), transform->m_position);
	transform->m_model *= glm::mat4_cast(glm::normalize(transform->m_rotation));
	transform->m_model = glm::scale(transform->m_model, transform->m_scale);
	transform->m_model *= transform->m_matrix;
}