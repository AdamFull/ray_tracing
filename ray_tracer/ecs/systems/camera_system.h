#pragma once

#include "base_system.h"
#include "ecs/components/fwdecl.h"

class CCameraSystem : public CBaseSystem
{
public:
	~CCameraSystem() override = default;

	void create(CRayEngine* engine) override;
	void update(CRayEngine* engine) override;
protected:
	void update_camera(entt::registry& registry, const glm::uvec2& extent, FCameraComponent* camera, FTransformComponent* transform);
	void recalculate_projection(FCameraComponent* camera);
	void recalculate_view(FCameraComponent* camera, FTransformComponent* transform);
	void recalculate_ray_directions(FCameraComponent* camera);
};