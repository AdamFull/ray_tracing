#pragma once

#include <tiny_gltf.h>
#include "light_source.h"
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

	size_t get_area_light_index(float index) const;
	const CTriangle& get_area_light(size_t index) const;
	float get_area_light_probability() const;

	size_t get_light_index(float index) const;
	const std::unique_ptr<CLightSource>& get_light(size_t index) const;
	float get_light_probability() const;

	entt::registry& get_registry();
	const entt::entity& get_root();

private:
	void load_gltf_scene(const std::filesystem::path& path, uint32_t scene_index);

	void load_samplers(const tinygltf::Model& model);
	void load_images(const tinygltf::Model& model);
	void load_image(tinygltf::Image* image, const int imageIndex, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size);
	void load_textures(const tinygltf::Model& model);
	void load_materials(const tinygltf::Model& model);

	void load_node(const entt::entity& parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale);
	void load_mesh_component(const entt::entity& target, const tinygltf::Node& node, const tinygltf::Model& model);
	void load_camera_component(const entt::entity& target, const tinygltf::Node& node, const tinygltf::Model& model);
	void load_light_component(const entt::entity& target, uint32_t light_index, const tinygltf::Node& node, const tinygltf::Model& model);

	static bool load_image_from_memory(tinygltf::Image* image, const int imageIndex, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* userData);
private:
	entt::registry m_registry{};
	entt::entity m_root{};

	//CBVHTree* m_pBVHTree{ nullptr };
	CBVHTreeNew* m_pBVHTree{ nullptr };

	std::vector<resource_id_t> m_vSamplerIds{};
	std::vector<resource_id_t> m_vImageIds{};
	std::vector<resource_id_t> m_vTextureIds{};
	std::vector<resource_id_t> m_vMaterialIds{};
	std::vector<size_t> m_vLightIds{};

	std::vector<std::unique_ptr<CLightSource>> m_vLightSources{};

	std::filesystem::path m_parentPath{};

	CResourceManager* m_pResourceManager{ nullptr };
};