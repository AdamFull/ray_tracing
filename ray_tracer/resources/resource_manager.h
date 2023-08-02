#pragma once

#include "resource_container.hpp"

#include "sampler.h"
#include "image.h"
#include "material.h"
#include "vertex_buffer.h"

class CResourceManager
{
public:
	CResourceManager();

	void create();

	// Sampler resource control
	resource_id_t add_sampler(const std::string& name, std::unique_ptr<CSampler>&& imageptr);
	const std::unique_ptr<CSampler>& get_sampler(const std::string& name) const;
	const std::unique_ptr<CSampler>& get_sampler(resource_id_t id) const;

	// Image resource control
	resource_id_t add_image(const std::string& name, std::unique_ptr<CImage>&& imageptr);
	resource_id_t add_image(const std::string& name, const std::filesystem::path& filepath);
	resource_id_t get_image_id(const std::string& name);
	void increment_image_usage(resource_id_t id);
	const std::unique_ptr<CImage>& get_image(const std::string& name) const;
	const std::unique_ptr<CImage>& get_image(resource_id_t id) const;

	// Material resource control
	resource_id_t add_material(const std::string& name, std::unique_ptr<CMaterial>&& material);
	const std::unique_ptr<CMaterial>& get_material(const std::string& name) const;
	const std::unique_ptr<CMaterial>& get_material(resource_id_t id) const;

	// Vertex buffer resource control
	resource_id_t add_vertex_buffer(const std::string& name);
	const std::unique_ptr<CVertexBuffer>& get_vertex_buffer(const std::string& name) const;
	const std::unique_ptr<CVertexBuffer>& get_vertex_buffer(resource_id_t id) const;

private:
	std::unique_ptr<resource_container<CSampler>> m_pSamplerContainer{};
	std::unique_ptr<resource_container<CImage>> m_pImageContainer{};
	std::unique_ptr<resource_container<CMaterial>> m_pMaterialContainer{};
	std::unique_ptr<resource_container<CVertexBuffer>> m_pVertexBufferContainer{};
};