#pragma once

#include "lib/tiny_gltf.h"
#include "shared.h"
#include "ecs/components/fwdecl.h"
#include "bvh_tree.h"

//class Sphere : public Hittable
//{
//public:
//	Sphere() = default;
//	~Sphere() override = default;
//	Sphere(const glm::vec3& position, const float radius, int64_t material);
//
//	virtual bool hit(const Ray& r, float t_min, float t_max, HitResult& hit) const override;
//private:
//	glm::vec3 _position{};
//	float _radius{ 0.5f };
//	int64_t _material_id{ -1 };
//};

class CResourceManager;

class CScene
{
public:
	CScene() = default;
	CScene(CResourceManager* resource_manager);
	~CScene();

	void create(const std::filesystem::path& scenepath);
	void build_acceleration();

	bool trace_ray(const FRay& ray, float t_min, float t_max, FHitResult& hit_result);

	entt::registry& get_registry();
	const entt::entity& get_root();

private:
	void load_gltf_scene(const std::filesystem::path& path, uint32_t scene_index);

	void load_samplers(const tinygltf::Model& model);
	resource_id_t load_texture(const std::filesystem::path& filepath);
	void load_textures(const tinygltf::Model& model);
	void load_materials(const tinygltf::Model& model);

	void load_node(const entt::entity& parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale);
	void load_mesh_component(const entt::entity& target, const tinygltf::Node& node, const tinygltf::Model& model);
	void load_camera_component(const entt::entity& target, const tinygltf::Node& node, const tinygltf::Model& model);
	void load_light_component(const entt::entity& target, uint32_t light_index, const tinygltf::Node& node, const tinygltf::Model& model);
private:
	entt::registry m_registry{};
	entt::entity m_root{};

	//CBVHTree* m_pBVHTree{ nullptr };
	CBVHTreeNew* m_pBVHTree{ nullptr };

	std::vector<resource_id_t> m_vSamplerIds{};
	std::vector<resource_id_t> m_vTextureIds{};
	std::vector<resource_id_t> m_vMaterialIds{};

	std::filesystem::path m_parentPath{};

	CResourceManager* m_pResourceManager{ nullptr };
};