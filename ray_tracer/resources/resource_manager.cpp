#include "resource_manager.h"

CResourceManager::CResourceManager()
{
	create();
}

void CResourceManager::create()
{
	m_pSamplerContainer = std::make_unique<resource_container<CSampler>>();
	m_pImageContainer = std::make_unique<resource_container<CImage>>();
	m_pMaterialContainer = std::make_unique<resource_container<CMaterial>>();
	m_pVertexBufferContainer = std::make_unique<resource_container<CVertexBuffer>>();
}

resource_id_t CResourceManager::add_sampler(const std::string& name, std::unique_ptr<CSampler>&& samplerptr)
{
	return m_pSamplerContainer->add(name, std::move(samplerptr));
}

const std::unique_ptr<CSampler>& CResourceManager::get_sampler(const std::string& name) const
{
	return m_pSamplerContainer->get(name);
}

const std::unique_ptr<CSampler>& CResourceManager::get_sampler(resource_id_t id) const
{
	return m_pSamplerContainer->get(id);
}


resource_id_t CResourceManager::add_image(const std::string& name, std::unique_ptr<CImage>&& imageptr)
{
	return m_pImageContainer->add(name, std::move(imageptr));
}

resource_id_t CResourceManager::add_image(const std::string& name, const std::filesystem::path& filepath)
{
	auto image_id = m_pImageContainer->get_id(name);

	if (image_id != invalid_index)
	{
		m_pImageContainer->increment(image_id);
		return image_id;
	}

	return add_image(name, std::make_unique<CImage>(filepath));
}

resource_id_t CResourceManager::get_image_id(const std::string& name)
{
	return m_pImageContainer->get_id(name);
}

void CResourceManager::increment_image_usage(resource_id_t id)
{
	return m_pImageContainer->increment(id);
}

const std::unique_ptr<CImage>& CResourceManager::get_image(const std::string& name) const
{
	return m_pImageContainer->get(name);
}

const std::unique_ptr<CImage>& CResourceManager::get_image(resource_id_t id) const
{
	return m_pImageContainer->get(id);
}


resource_id_t CResourceManager::add_material(const std::string& name, std::unique_ptr<CMaterial>&& material)
{
	return m_pMaterialContainer->add(name, std::move(material));
}

const std::unique_ptr<CMaterial>& CResourceManager::get_material(const std::string& name) const
{
	return m_pMaterialContainer->get(name);
}

const std::unique_ptr<CMaterial>& CResourceManager::get_material(resource_id_t id) const
{
	return m_pMaterialContainer->get(id);
}


resource_id_t CResourceManager::add_vertex_buffer(const std::string& name)
{
	return m_pVertexBufferContainer->add(name, std::make_unique<CVertexBuffer>());
}

const std::unique_ptr<CVertexBuffer>& CResourceManager::get_vertex_buffer(const std::string& name) const
{
	return m_pVertexBufferContainer->get(name);
}

const std::unique_ptr<CVertexBuffer>& CResourceManager::get_vertex_buffer(resource_id_t id) const
{
	return m_pVertexBufferContainer->get(id);
}