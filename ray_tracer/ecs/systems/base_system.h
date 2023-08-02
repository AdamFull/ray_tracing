#pragma once

class CRayEngine;

class CBaseSystem
{
public:
	virtual ~CBaseSystem() = default;

	virtual void create(CRayEngine* engine) = 0;
	virtual void update(CRayEngine* engine) = 0;
};