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
};