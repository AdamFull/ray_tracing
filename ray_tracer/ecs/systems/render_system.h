#pragma once

#include "base_system.h"
#include "ecs/components/fwdecl.h"
#include "shared.h"

class CScene;

class CRenderSystem : public CBaseSystem
{
public:
	~CRenderSystem() override = default;

	void create(CRayEngine* engine) override;
	void update(CRayEngine* engine) override;
private:
	void trace_ray(CRayEngine* engine, FCameraComponent* camera, FTransformComponent* camera_transform, uint32_t ray_index);
	glm::vec3 hit_pixel(CRayEngine* engine, FRay ray, int32_t bounces);
};