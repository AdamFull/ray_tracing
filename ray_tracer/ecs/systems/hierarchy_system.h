#pragma once

#include "base_system.h"
#include "ecs/components/fwdecl.h"

class CHierarchySystem : public CBaseSystem
{
public:
	~CHierarchySystem() override = default;

	void create(CRayEngine* engine) override;
	void update(CRayEngine* engine) override;
private:
	void build_hierarchy(entt::registry& registry, const entt::entity& node);
	void initialize_matrix(FTransformComponent* transform);
};