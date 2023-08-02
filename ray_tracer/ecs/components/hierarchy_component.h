#pragma once

struct FHierarchyComponent
{
	FHierarchyComponent(const std::string& name) : m_name(name) {}

	std::string m_name{};

	entt::entity parent{ entt::null };
	std::vector<entt::entity> children{};
};