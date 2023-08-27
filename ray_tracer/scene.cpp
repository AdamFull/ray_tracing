#include "scene.h"

#include <iostream>

#include <glm/gtc/type_ptr.inl>

#include "ecs/components/components.h"
#include "resources/resource_manager.h"

#include "resources/material.h"

#define TINYGLTF_IMPLEMENTATION
#include "lib/tiny_gltf.h"

double getDoubleValueOrDefault(const std::string& name, const tinygltf::Value& val, double _default = 0.0)
{
	if (val.Has(name))
	{
		auto obj = val.Get(name);
		return obj.GetNumberAsDouble();
	}

	return _default;
}

std::string url_decode(const std::string& str)
{
	std::string ret;
	char ch;
	int i, ii, len = str.length();

	for (i = 0; i < len; i++)
	{
		if (str[i] != '%')
		{
			if (str[i] == '+')
				ret += ' ';
			else
				ret += str[i];
		}
		else
		{
			sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			ret += ch;
			i = i + 2;
		}
	}
	return ret;
}

entt::entity create_node(entt::registry& registry, const std::string& name)
{
	auto entity = registry.create();
	registry.emplace<FTransformComponent>(entity, FTransformComponent{});
	registry.emplace<FHierarchyComponent>(entity, FHierarchyComponent{ name });
	std::cout << std::format("Entity with id {} was created\n", static_cast<uint32_t>(entity));
	return entity;
}

void attach_child(entt::registry& registry, entt::entity parent, entt::entity child)
{
	if (parent == child)
	{
		std::cout << std::format("Trying to add parent to parent. Parent entity {}, child entity {}.\n", static_cast<uint32_t>(parent), static_cast<uint32_t>(child));
		return;
	}

	auto& phierarchy = registry.get<FHierarchyComponent>(parent);
	phierarchy.children.emplace_back(child);

	auto& chierarchy = registry.get<FHierarchyComponent>(child);
	chierarchy.parent = parent;
}

bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	return true;
}

CScene::CScene(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

CScene::~CScene()
{
	delete m_pBVHTree;
	m_pBVHTree = nullptr;
}

void CScene::create(const std::filesystem::path& scenepath)
{
	// Create root node
	m_root = create_node(m_registry, "scene_root");

	m_parentPath = scenepath.parent_path();

	//m_pBVHTree = new CBVHTree();
	m_pBVHTree = new CBVHTreeNew();

	// Load scene
	load_gltf_scene(scenepath, 0u);
}

void CScene::build_acceleration()
{
	m_pBVHTree->create();
}

bool CScene::trace_ray(const FRay& ray, float t_min, float t_max, FHitResult& hit_result)
{
	return m_pBVHTree->hit(ray, t_min, t_max, hit_result);
}

size_t CScene::get_light_index(float index) const
{
	return m_vLightIds.at(static_cast<size_t>(index * m_vLightIds.size() - 1ull));
}

const CTriangle& CScene::get_light(size_t index) const
{
	return m_pBVHTree->get_triangle(index);
}

float CScene::get_light_probability() const
{
	return 1.f / static_cast<float>(m_vLightIds.size());
}

entt::registry& CScene::get_registry()
{
	return m_registry;
}

const entt::entity& CScene::get_root()
{
	return m_root;
}

void CScene::load_gltf_scene(const std::filesystem::path& path, uint32_t scene_index)
{
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;

	std::string error, warning;
	gltfContext.SetImageLoader(&loadImageDataFuncEmpty, this);

	bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, path.string());

	if (!warning.empty())
		std::cout << std::format("\nWarnings while loading gltf scene \"{}\": \n{}", path.string(), warning);
	if (!error.empty())
		std::cerr << std::format("\nErrors while loading gltf scene \"{}\": \n{}", path.string(), error);

	if (!fileLoaded)
	{
		std::cerr << "Failed to load file\n";
		return;
	}

	load_samplers(gltfModel);
	load_textures(gltfModel);
	load_materials(gltfModel);

	// Load scene nodes
	const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
	for (auto& node_idx : scene.nodes)
	{
		const tinygltf::Node node = gltfModel.nodes[node_idx];
		load_node(m_root, node, node_idx, gltfModel, 1.0);
	}
}

void CScene::load_samplers(const tinygltf::Model& model)
{
	for (auto& sampler : model.samplers)
	{
		auto new_sampler = std::make_unique<CSampler>(m_pResourceManager);
		new_sampler->create(
			static_cast<EFilterMode>(sampler.minFilter),
			static_cast<EFilterMode>(sampler.magFilter),
			static_cast<EWrapMode>(sampler.wrapS),
			static_cast<EWrapMode>(sampler.wrapT));

		m_vSamplerIds.emplace_back(m_pResourceManager->add_sampler(sampler.name, std::move(new_sampler)));
	}
}

resource_id_t CScene::load_texture(const std::filesystem::path& filepath)
{
	auto name = filepath.filename();
	return m_pResourceManager->add_image(name.string(), filepath);
}

void CScene::load_textures(const tinygltf::Model& model)
{
	for (auto& texture : model.textures)
	{
		auto& image = model.images.at(texture.source);
		auto texture_path = m_parentPath / url_decode(image.uri);

		auto filename = texture_path.filename().string();

		auto image_id = m_pResourceManager->get_image_id(filename);

		if (image_id != invalid_index)
			m_pResourceManager->increment_image_usage(image_id);
		else
		{
			auto new_image = std::make_unique<CImage>();
			new_image->load(texture_path);
			image_id = m_pResourceManager->add_image(filename, std::move(new_image));
		}

		auto& image_ptr = m_pResourceManager->get_image(image_id);
		image_ptr->set_sampler(m_vSamplerIds[texture.sampler]);
		
		m_vTextureIds.emplace_back(image_id);
	}
}

void CScene::load_materials(const tinygltf::Model& model)
{
	if (model.materials.empty())
	{
		m_vMaterialIds.emplace_back(m_pResourceManager->add_material("default_diffuse_material", std::make_unique<CMaterial>(m_pResourceManager)));
		return;
	}

	uint32_t matIndex{ 0 };
	for (auto& mat : model.materials)
	{
		bool is_metallicRoughness{ false };
		bool is_emissive{ false };

		FMaterialCreateInfo material_ci{};

		if (mat.values.find("baseColorTexture") != mat.values.end())
		{
			auto texture = mat.values.at("baseColorTexture");
			material_ci.m_textures.emplace(ETextureType::eAlbedo, m_vTextureIds.at(texture.TextureIndex()));
		}

		if (mat.values.find("metallicRoughnessTexture") != mat.values.end())
		{
			auto texture = mat.values.at("metallicRoughnessTexture");
			material_ci.m_textures.emplace(ETextureType::eMetallRoughness, m_vTextureIds.at(texture.TextureIndex()));
			is_metallicRoughness = true;
		}

		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end())
		{
			auto texture = mat.additionalValues.at("normalTexture");
			material_ci.m_fNormalMapScale = static_cast<float>(texture.TextureScale());
			material_ci.m_textures.emplace(ETextureType::eNormal, m_vTextureIds.at(texture.TextureIndex()));
		}

		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end())
		{
			auto texture = mat.additionalValues.at("occlusionTexture");
			material_ci.m_fOcclusionStrength = static_cast<float>(texture.TextureStrength());
			material_ci.m_textures.emplace(ETextureType::eAmbientOcclusion, m_vTextureIds.at(texture.TextureIndex()));
		}

		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
		{
			auto texture = mat.additionalValues.at("emissiveTexture");
			material_ci.m_textures.emplace(ETextureType::eEmission, m_vTextureIds.at(texture.TextureIndex()));
			is_emissive = true;
		}

		if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
		{
			material_ci.m_emissiveFactor = glm::make_vec3(mat.additionalValues.at("emissiveFactor").ColorFactor().data());
			if(material_ci.m_emissiveFactor != glm::vec3(0.f))
				is_emissive = true;
		}

		if (mat.values.find("roughnessFactor") != mat.values.end())
		{
			material_ci.m_fRoughnessFactor = static_cast<float>(mat.values.at("roughnessFactor").Factor());
			is_metallicRoughness = true;
		}

		if (mat.values.find("metallicFactor") != mat.values.end())
		{
			material_ci.m_fMetallicFactor = static_cast<float>(mat.values.at("metallicFactor").Factor());
			if(material_ci.m_fMetallicFactor > 0.f)
				is_metallicRoughness = true;
		}

		if (mat.values.find("baseColorFactor") != mat.values.end())
			material_ci.m_baseColorFactor = glm::make_vec4(mat.values.at("baseColorFactor").ColorFactor().data());

		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
		{
			tinygltf::Parameter param = mat.additionalValues.at("alphaMode");
			if (param.string_value == "BLEND")
				material_ci.m_alphaMode = EAlphaMode::eBlend;
			else  if (param.string_value == "MASK")
				material_ci.m_alphaMode = EAlphaMode::eMask;
			else
				material_ci.m_alphaMode = EAlphaMode::eOpaque;
		}

		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end())
			material_ci.m_alphaCutoff = static_cast<float>(mat.additionalValues.at("alphaCutoff").Factor());

		for (auto& [name, data] : mat.extensions)
		{
			if (name == "KHR_materials_emissive_strength")
			{
				material_ci.m_emissiveStrength = getDoubleValueOrDefault("emissiveStrength", data, 1.0);
				is_emissive = true;
			}
		}

		// Create concrete material
		std::unique_ptr<CMaterial> new_material{};
		//if (is_emissive)
		//	new_material = std::make_unique<CEmissiveMaterial>(m_pResourceManager);
		//else
			new_material = std::make_unique<CMaterial>(m_pResourceManager);

		new_material->create(material_ci);

		m_vMaterialIds.emplace_back(m_pResourceManager->add_material(mat.name, std::move(new_material)));
	}
}

void CScene::load_node(const entt::entity& parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale)
{
	auto& parent_hierarchy = m_registry.get<FHierarchyComponent>(parent);

	auto obj_name = node.name.empty() ? parent_hierarchy.m_name + "_" + std::to_string(nodeIndex) : node.name;
	auto new_node = create_node(m_registry, obj_name);

	auto& node_transform = m_registry.get<FTransformComponent>(new_node);

	// Loading position data
	if (!node.translation.empty())
		node_transform.m_position = glm::make_vec3(node.translation.data());

	// Loading rotation data
	if (!node.rotation.empty())
		node_transform.m_rotation = glm::make_quat(node.rotation.data());

	// Loading scale data
	if (!node.scale.empty())
		node_transform.m_scale = glm::make_vec3(node.scale.data());
		//node_transform.m_scale = glm::vec3(1.f);

	// Load baked model matrix
	if (!node.matrix.empty())
		node_transform.m_matrix = glm::make_mat4(node.matrix.data());

	// Node with children
	if (node.children.size() > 0)
	{
		for (auto i = 0; i < node.children.size(); i++)
			load_node(new_node, model.nodes[node.children[i]], node.children[i], model, globalscale);
	}

	if (node.mesh > -1) 
		load_mesh_component(new_node, node, model);

	if (node.camera > -1) 
		load_camera_component(new_node, node, model);

	if (!node.extensions.empty())
	{
		auto light_support = node.extensions.find("KHR_lights_punctual");
		if (light_support != node.extensions.end())
		{
			auto& extension = light_support->second;
			auto source = extension.Get("light");
			load_light_component(new_node, source.GetNumberAsInt(), node, model);
		}
	}

	attach_child(m_registry, parent, new_node);
}

void CScene::load_mesh_component(const entt::entity& target, const tinygltf::Node& node, const tinygltf::Model& model)
{
	const tinygltf::Mesh& mesh = model.meshes[node.mesh];

	for (size_t j = 0; j < mesh.primitives.size(); j++)
	{
		std::vector<uint32_t> indexBuffer;
		std::vector<FVertex> vertexBuffer;

		const tinygltf::Primitive& primitive = mesh.primitives[j];

		uint32_t indexStart = static_cast<uint32_t>(0);
		uint32_t vertexStart = static_cast<uint32_t>(0);
		uint32_t indexCount = 0;
		uint32_t vertexCount = 0;

		bool bHasNormals{ false }, bHasTangents{ false };

		// Vertices
		{
			const float* bufferPos = nullptr;
			const float* bufferNormals = nullptr;
			const float* bufferTexCoords = nullptr;
			const float* bufferColors = nullptr;
			const float* bufferTangents = nullptr;
			uint32_t numColorComponents;
			const uint16_t* bufferJoints = nullptr;
			const float* bufferWeights = nullptr;

			// Position attribute is required
			assert(primitive.attributes.find("POSITION") != primitive.attributes.end());
			const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
			const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
			bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));

			// Load model normals
			if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
			{
				const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
				const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
				bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				bHasNormals = true;
			}

			// Load UV coordinates
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
			{
				const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
				bufferTexCoords = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
			}

			// Load mesh color data
			if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
			{
				const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
				const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
				// Color buffer are either of type vec3 or vec4
				numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
				bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
			}

			if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
			{
				const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
				const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
				bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
				bHasTangents = true;
			}

			vertexCount = static_cast<uint32_t>(posAccessor.count);

			for (size_t v = 0; v < posAccessor.count; v++)
			{
				FVertex vert{};
				vert.m_position = glm::make_vec3(&bufferPos[v * 3]);
				vert.m_normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
				vert.m_tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);

				vert.m_texcoord = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec2(0.0f);
				if (bufferColors)
				{
					switch (numColorComponents)
					{
					case 3:
						vert.m_color = glm::make_vec3(&bufferColors[v * 3]);
						break;
					case 4:
						vert.m_color = glm::make_vec3(&bufferColors[v * 4]);
						break;
					}
				}
				else
				{
					vert.m_color = glm::vec3(1.0f);
				}

				vertexBuffer.push_back(vert);
			}
		}

		// Indices
		if (primitive.indices >= 0)
		{
			const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

			indexCount = static_cast<uint32_t>(accessor.count);

			switch (accessor.componentType)
			{
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
			{
				uint32_t* buf = new uint32_t[accessor.count];
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
				for (size_t index = 0; index < accessor.count; index++)
					indexBuffer.push_back(buf[index] + vertexStart);
				delete[] buf;
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
			{
				uint16_t* buf = new uint16_t[accessor.count];
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
				for (size_t index = 0; index < accessor.count; index++)
					indexBuffer.push_back(buf[index] + vertexStart);
				delete[] buf;
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
			{
				uint8_t* buf = new uint8_t[accessor.count];
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
				for (size_t index = 0; index < accessor.count; index++)
					indexBuffer.push_back(buf[index] + vertexStart);
				delete[] buf;
				break;
			}
			default:
			{
				std::stringstream ss;
				ss << "Index component type " << accessor.componentType << " not supported!";
				std::cout << std::format("Warning while loading gltf scene: {}\n", ss.str());
				return;
			} break;
			}
		}

		resource_id_t material_id{ invalid_index };
		// Attaching materials
		if (!m_vMaterialIds.empty())
			material_id = primitive.material != invalid_index ? m_vMaterialIds.at(primitive.material) : m_vMaterialIds.back();

		// Calculate tangent
		if (!bHasTangents)
		{
			std::vector<glm::vec3> subtangents{};
			subtangents.resize(vertexBuffer.size());

			for (uint32_t index = 0u; index < indexBuffer.size(); index += 3u)
			{
				auto i0 = indexBuffer.at(index);
				auto i1 = indexBuffer.at(index + 1u);
				auto i2 = indexBuffer.at(index + 2u);

				auto& v0 = vertexBuffer.at(i0);
				auto& v1 = vertexBuffer.at(i1);
				auto& v2 = vertexBuffer.at(i2);

				auto edge1 = v1.m_position - v0.m_position;
				auto edge2 = v2.m_position - v0.m_position;

				glm::vec2 deltaUV1 = v1.m_texcoord - v0.m_texcoord;
				glm::vec2 deltaUV2 = v2.m_texcoord - v0.m_texcoord;

				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				glm::vec4 tangent;
				tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
				tangent.w = 0.f;

				v0.m_tangent += tangent;
				v1.m_tangent += tangent;
				v2.m_tangent += tangent;

				glm::vec3 tangent_2;
				tangent_2.x = f * (deltaUV1.x * edge2.x - deltaUV2.x * edge1.x);
				tangent_2.y = f * (deltaUV1.x * edge2.y - deltaUV2.x * edge1.y);
				tangent_2.z = f * (deltaUV1.x * edge2.z - deltaUV2.x * edge1.z);

				subtangents[i0] += tangent_2;
				subtangents[i1] += tangent_2;
				subtangents[i2] += tangent_2;
			}

			for (size_t index = 0ull; index < vertexBuffer.size(); ++index)
			{
				auto& vert = vertexBuffer[index];
				glm::vec3 t = glm::normalize(vert.m_tangent);
				vert.m_tangent = glm::vec4(glm::normalize(t - vert.m_normal * glm::dot(vert.m_normal, t)), 0.f);
				vert.m_tangent.w = glm::dot(glm::cross(vert.m_normal, subtangents[index]), t) < 0.f ? -1.f : 1.f;
			}
		}

		auto& material = m_pResourceManager->get_material(material_id);
		bool is_light_emitter = material->can_emit_light() && !material->can_scatter_light();

		for (uint32_t index = 0u; index < indexBuffer.size(); index += 3u)
		{
			auto i0 = indexBuffer.at(index);
			auto i1 = indexBuffer.at(index + 1u);
			auto i2 = indexBuffer.at(index + 2u);

			auto& v0 = vertexBuffer.at(i0);
			auto& v1 = vertexBuffer.at(i1);
			auto& v2 = vertexBuffer.at(i2);

			auto triangle_index = m_pBVHTree->size();
			m_pBVHTree->emplace(CTriangle(m_registry, target, material_id, v0, v1, v2, triangle_index));

			if (is_light_emitter)
				m_vLightIds.emplace_back(triangle_index);
		}
	}
}

void CScene::load_camera_component(const entt::entity& target, const tinygltf::Node& node, const tinygltf::Model& model)
{
	const tinygltf::Camera camera = model.cameras[node.camera];
	FCameraComponent cameraComponent;

	if (camera.type == "orthographic")
	{
		cameraComponent.m_type = ECameraType::eOrthographic;
		cameraComponent.m_xmag = camera.orthographic.xmag;
		cameraComponent.m_ymag = camera.orthographic.ymag;
		cameraComponent.m_near = camera.orthographic.znear;
		cameraComponent.m_far = camera.orthographic.zfar;
	}
	else if (camera.type == "perspective")
	{
		cameraComponent.m_type = ECameraType::ePerspective;
		cameraComponent.m_fov = camera.perspective.yfov;
		cameraComponent.m_near = camera.perspective.znear;
		cameraComponent.m_far = camera.perspective.zfar;
	}

	m_registry.emplace<FCameraComponent>(target, cameraComponent);
}

void CScene::load_light_component(const entt::entity& target, uint32_t light_index, const tinygltf::Node& node, const tinygltf::Model& model)
{
	const tinygltf::Light light = model.lights[light_index];

	glm::vec3 color;

	if (light.color.empty())
		color = glm::vec3(1.f);
	else
		color = glm::make_vec3(light.color.data());

	if (light.type == "directional")
	{
		FDirectionalLightComponent lightComponent;
		lightComponent.m_color = color;
		lightComponent.m_intencity = light.intensity;
		m_registry.emplace<FDirectionalLightComponent>(target, lightComponent);
	}
	else if (light.type == "spot")
	{
		FSpotLightComponent lightComponent;
		lightComponent.m_color = color;
		lightComponent.m_innerAngle = light.spot.innerConeAngle;
		lightComponent.m_outerAngle = light.spot.outerConeAngle;
		lightComponent.m_intencity = light.intensity;
		m_registry.emplace<FSpotLightComponent>(target, lightComponent);
	}
	else if (light.type == "point")
	{
		FPointLightComponent lightComponent;
		lightComponent.m_color = color;
		lightComponent.m_intencity = light.intensity;
		lightComponent.m_radius = light.range;
		m_registry.emplace<FPointLightComponent>(target, lightComponent);
	}
}