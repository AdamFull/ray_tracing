#pragma once

#include "base_system.h"
#include "ecs/components/fwdecl.h"

class CCameraSystem : public CBaseSystem
{
public:
	~CCameraSystem() override = default;

	void create(CRayEngine* engine) override;
	void update(CRayEngine* engine) override;

	// Public so the interactive preview can recompute the projection/view/ray directions
	// for a camera at an arbitrary viewport extent (preview vs. full-render resolution).
	void update_camera(entt::registry& registry, const glm::uvec2& extent, FCameraComponent* camera, FTransformComponent* transform);
protected:
	void recalculate_projection(FCameraComponent* camera);
	void recalculate_view(FCameraComponent* camera, FTransformComponent* transform);
	void recalculate_ray_directions(FCameraComponent* camera);
};