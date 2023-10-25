#pragma once

#include "hittable.h"
#include <stack>

struct FBVHNode
{
	FAxixAlignedBoundingBox m_aabb{};
	uint32_t m_left{};
	uint32_t m_count{};

	bool is_leaf() const { return m_count > 0u; }

	float cost() const
	{
		auto extent = m_aabb.extent();
		return (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x) * static_cast<float>(m_count);
	}
};

struct FBVHTreeBin
{
	FAxixAlignedBoundingBox m_bounds{}; 
	uint32_t m_count{ 0u };
};

class CBVHTreeNew
{
	struct FBuildJob
	{
		uint32_t m_nodeIdx{};
		FAxixAlignedBoundingBox m_centroid{};
	};
public:
	~CBVHTreeNew();

	void create();
	void emplace(const CTriangle& triangle);
	size_t size() const;
	const CTriangle& get_triangle(size_t index) const;

	bool hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const;
private:
	bool hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result, uint32_t node_idx) const;
	void build();
	void grow(uint32_t node_idx, FAxixAlignedBoundingBox& aabb);
	void subdivide(uint32_t node_idx, uint32_t depth, FAxixAlignedBoundingBox& centroid);
	float find_best_split(FBVHNode& node, uint32_t& axis, uint32_t& split_pos, FAxixAlignedBoundingBox& centroid);
protected:
	std::vector<FBVHNode> m_vNodes{};
	std::vector<CTriangle> m_vHittables{};
	std::vector<uint32_t> m_vIndices{};

	uint32_t m_size{ 0u };
};