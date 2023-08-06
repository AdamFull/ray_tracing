#include "engine.h"

#include "ecs/systems/systems.h"

#include "ggx.hpp"

void CRayEngine::create(const std::filesystem::path& scenepath, uint32_t width, uint32_t heigth, uint32_t sample_count)
{
	m_pResourceManager = std::make_unique<CResourceManager>();

	m_pRenderer = std::make_unique<CRenderCore>(m_pResourceManager.get());
	m_pRenderer->create(width, heigth, sample_count);

	m_pScene = std::make_unique<CScene>(m_pResourceManager.get());
	m_pScene->create(scenepath, generate_brdf_lut(1024u, 1024u));

	// Create systems
	m_vSystems.emplace_back(std::make_unique<CHierarchySystem>());
	m_vSystems.emplace_back(std::make_unique<CCameraSystem>());
	m_vSystems.emplace_back(std::make_unique<CRenderSystem>());

	for (auto& system : m_vSystems)
		system->create(this);
}

void CRayEngine::update()
{
	for (auto& system : m_vSystems)
		system->update(this);
}

const std::unique_ptr<CResourceManager>& CRayEngine::get_resource_manager() const
{
	return m_pResourceManager;
}

const std::unique_ptr<CRenderCore>& CRayEngine::get_renderer() const
{
	return m_pRenderer;
}

const std::unique_ptr<CScene>& CRayEngine::get_scene() const
{
	return m_pScene;
}

resource_id_t CRayEngine::generate_brdf_lut(uint32_t size, uint32_t samples)
{
	auto image = std::make_unique<CImage>();
	image->create(size, size);

	std::vector<uint32_t> parallel_iter{};
	parallel_iter.resize(size * size);
	std::iota(parallel_iter.begin(), parallel_iter.end(), 0u);

	std::for_each(std::execution::par, parallel_iter.begin(), parallel_iter.end(),
		[&image, size, samples](uint32_t idx)
		{
			auto x = idx % size;
			auto y = idx / size;
			math::vec2 uv{ static_cast<float>(x) / static_cast<float>(size), static_cast<float>(y) / static_cast<float>(size) };

			auto lut = compute_brdf(uv.x, 1.f - uv.y, samples);
			image->set_pixel(x, y, pack_color_u32(math::to_vec4(lut, 0.f, 1.f)));
		});

	auto sampler = std::make_unique<CSampler>(m_pResourceManager.get());
	sampler->create(EFilterMode::eNearest, EFilterMode::eNearest, EWrapMode::eRepeat, EWrapMode::eRepeat);
	image->set_sampler(m_pResourceManager->add_sampler("brdf_lut_sampler", std::move(sampler)));

	return m_pResourceManager->add_image("brdf_lut_texture", std::move(image));
}