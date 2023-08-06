#pragma once

#include "hittable.h"

class CBVHNode : public CHittable
{
public:
	CBVHNode() = default;
	CBVHNode(std::vector<CTriangle*>& hittables, size_t start, size_t end);
	~CBVHNode() override;

	bool hit(const FRay& r, float t_min, float t_max, FHitResult& hit_result) const override;
	bool bounds(FAxixAlignedBoundingBox& output_box) const override;
private:
	CTriangle* m_pLeafLeft{ nullptr };
	CTriangle* m_pLeafRight{ nullptr };

	CHittable* m_pLeft{ nullptr };
	CHittable* m_pRight{ nullptr };
};

class CBVHTree
{
public:
	~CBVHTree();

	void create();
	void emplace(CTriangle* hittable);

	bool hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const;
private:
	CBVHNode* m_pRoot{ nullptr };
	std::vector<CTriangle*> m_vHittables{};
};


//struct FBVHNode
//{
//	FAxixAlignedBoundingBox m_aabb{ std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
//	uint32_t m_left{ 0u };
//	uint32_t m_start{ 0u }, m_count{ 0u };
//
//	bool is_leaf() const
//	{
//		return m_count > 0u;
//	}
//};
//
//template<class _Ty>
//class CBVHTree
//{
//public:
//	void emplace(_Ty&& primitive)
//	{
//		m_vPrimitives.emplace_back(std::forward<_Ty>(primitive));
//	}
//
//	void create()
//	{
//		for (auto& primitive : m_vPrimitives)
//			primitive.create();
//
//		const size_t element_count = m_vPrimitives.size();
//		m_vNodes.resize(element_count * 2ull - 1ull);
//		m_vPrimitiveIndices.resize(element_count);
//		std::iota(m_vPrimitiveIndices.begin(), m_vPrimitiveIndices.end(), 0u);
//
//		auto& root = m_vNodes[m_rootNode];
//		root.m_count = static_cast<uint32_t>(element_count);
//
//		update_bounds(m_rootNode);
//		subdivide(m_rootNode);
//	}
//
//	bool hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result, uint32_t node_index = 0u)
//	{
//		auto& node = m_vNodes[node_index];
//		if(!node.m_aabb.hit(ray, t_min, t_max))
//			return false;
//
//		float closest_hit{ t_max };
//		bool hit_left{ false }, hit_right{ false };
//
//		if(node.is_leaf())
//		{
//			bool hit_something{ false };
//			for (uint32_t idx = 0u; idx < node.m_count; ++idx)
//			{
//				auto primitive_index = m_vPrimitiveIndices[node.m_start + idx];
//				auto& primitive = m_vPrimitives[primitive_index];
//				if (primitive.hit(ray, t_min, closest_hit, hit_result))
//				{
//					closest_hit = hit_result.m_distance;
//					hit_something = true;
//				}
//			}
//
//			return hit_something;
//		}
//		else
//		{
//			hit_left = hit(ray, t_min, closest_hit, hit_result, node.m_left);
//			hit_right = hit(ray, t_min, closest_hit, hit_result, node.m_left + 1u);
//		}
//
//		return hit_left || hit_right;
//	}
//protected:
//	float estimate_SAH(FBVHNode& node, uint32_t axis, float pos)
//	{
//		FAxixAlignedBoundingBox left{}, right{};
//		uint32_t left_count{ 0u }, right_count{ 0u };
//
//		for (uint32_t idx = 0u; idx < node.m_count; ++idx)
//		{
//			auto& triangle = m_vPrimitives[m_vPrimitiveIndices[node.m_start + idx]];
//			if (triangle.centroid()[axis] < pos)
//			{
//				left_count++;
//				FAxixAlignedBoundingBox other{};
//				triangle.bounds(other);
//				left.grow(other);
//			}
//			else
//			{
//				right_count++;
//				FAxixAlignedBoundingBox other{};
//				triangle.bounds(other);
//				right.grow(other);
//			}
//		}
//
//		float cost = static_cast<float>(left_count) * left.area() + static_cast<float>(right_count) * right.area();
//		return cost > 0.f ? cost : 1e30f;
//	}
//
//	void update_bounds(uint32_t node_index)
//	{
//		auto& node = m_vNodes[node_index];
//
//		for (uint32_t idx = node.m_start; idx < node.m_count; ++idx)
//		{
//			auto primitive_idx = m_vPrimitiveIndices[idx];
//			auto& primitive = m_vPrimitives[primitive_idx];
//
//			FAxixAlignedBoundingBox other{};
//			primitive.bounds(other);
//			node.m_aabb.m_min = glm::min(node.m_aabb.m_min, other.m_min);
//			node.m_aabb.m_max = glm::max(node.m_aabb.m_max, other.m_max);
//		}
//	}
//
//	void subdivide(uint32_t node_index)
//	{
//		auto& node = m_vNodes[node_index];
//
//		if (node.m_count <= 1u)
//			return;
//
//		auto extent = node.m_aabb.extent();
//
//		uint32_t best_axis{ 0u };
//		float best_pos{ 0.f };
//		float best_cost{ std::numeric_limits<float>::max() };
//
//		std::vector<uint32_t> triangle_range{};
//		triangle_range.resize(node.m_count);
//
//		for (uint32_t axis = 0u; axis < 3u; ++axis)
//		{
//			std::for_each(std::execution::par, triangle_range.begin(), triangle_range.end(), [this, &best_axis, &best_pos, &best_cost, axis, &node](uint32_t idx)
//				{
//					auto& triangle = m_vPrimitives[m_vPrimitiveIndices[node.m_start + idx]];
//					float candidate_pos = triangle.centroid()[axis];
//					float cost = estimate_SAH(node, axis, candidate_pos);
//					if (cost < best_cost)
//					{
//						best_pos = candidate_pos;
//						best_axis = axis;
//						best_cost = cost;
//					}
//				});
//		}
//
//		// Select divider axis
//		uint32_t axis{ best_axis };
//		float split_pos{ best_pos };
//
//		uint32_t idx = node.m_start;
//		uint32_t count = idx + node.m_count - 1u;
//
//		while (idx <= count)
//		{
//			if (m_vPrimitives[m_vPrimitiveIndices[idx]].centroid()[axis] < split_pos)
//				++idx;
//			else
//				std::swap(m_vPrimitiveIndices[idx], m_vPrimitiveIndices[count--]);
//		}
//
//		uint32_t left_count = idx - node.m_start;
//		if (left_count == 0u || left_count == node.m_count)
//			return;
//
//		uint32_t left_child = m_nodeCount++;
//		m_vNodes[left_child].m_start = node.m_start;
//		m_vNodes[left_child].m_count = left_count;
//
//		uint32_t rigth_child = m_nodeCount++;
//		m_vNodes[rigth_child].m_start = idx;
//		m_vNodes[rigth_child].m_count = node.m_count - left_count;
//
//		node.m_left = left_child;
//		node.m_count = 0u;
//
//		update_bounds(left_child);
//		update_bounds(rigth_child);
//
//		subdivide(left_child);
//		subdivide(rigth_child);
//	}
//private:
//	std::vector<_Ty> m_vPrimitives{};
//	std::vector<uint32_t> m_vPrimitiveIndices{};
//	std::vector<FBVHNode> m_vNodes{};
//
//	uint32_t m_rootNode{ 0u };
//	uint32_t m_nodeCount{ 1u };
//};