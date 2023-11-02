#pragma once

#include "shared.h"
#include "ecs/components/fwdecl.h"

class CLightSource
{
public:
	virtual ~CLightSource() = default;

	virtual void create(entt::entity root, const entt::registry& registry);

	virtual glm::vec3 get_direction(const FHitResult& hit_result) const = 0;
	virtual float get_distance(const FHitResult& hit_result) const = 0;
	virtual float get_pdf(const FHitResult& hit_result) const = 0;

	virtual glm::vec3 get_color(const FHitResult& hit_result) const = 0;
protected:
	entt::entity m_root{ entt::null };
	const entt::registry* m_pRegistry{ nullptr };
	const FTransformComponent* m_transform{ nullptr };
};

class CDirectionalLightSource : public CLightSource
{
public:
	~CDirectionalLightSource() override = default;

	void create(entt::entity root, const entt::registry& registry) override;

	glm::vec3 get_direction(const FHitResult& hit_result) const override;
	float get_distance(const FHitResult& hit_result) const override;
	float get_pdf(const FHitResult& hit_result) const override;

	glm::vec3 get_color(const FHitResult& hit_result) const override;
private:
	const FDirectionalLightComponent* m_light{ nullptr };
};

class CPointLightSource : public CLightSource
{
public:
	~CPointLightSource() override = default;

	void create(entt::entity root, const entt::registry& registry) override;

	glm::vec3 get_direction(const FHitResult& hit_result) const override;
	float get_distance(const FHitResult& hit_result) const override;
	float get_pdf(const FHitResult& hit_result) const override;

	glm::vec3 get_color(const FHitResult& hit_result) const override;
private:
	const FPointLightComponent* m_light{ nullptr };
};

class CSpotLightSource : public CLightSource
{
public:
	~CSpotLightSource() override = default;

	void create(entt::entity root, const entt::registry& registry) override;

	glm::vec3 get_direction(const FHitResult& hit_result) const override;
	float get_distance(const FHitResult& hit_result) const override;
	float get_pdf(const FHitResult& hit_result) const override;

	glm::vec3 get_color(const FHitResult& hit_result) const override;
private:
	const FSpotLightComponent* m_light{ nullptr };
};