#include "bvh_tree.h"

#include "util.h"

#include <iostream>

constexpr uint32_t bins = 8u;

void CBVHTreeNew::create()
{
    // Initialize node array
    m_vNodes.resize(m_vTriangles.size() * 2ull + 64ull);

    // Initialize triangle indices
    m_vIndices.resize(m_vTriangles.size());
    std::iota(m_vIndices.begin(), m_vIndices.end(), 0u);

    // Initialize triangles
    for (auto& triangle : m_vTriangles)
        triangle.create();

    // Build tree
    build();
}

void CBVHTreeNew::emplace(CTriangle triangle)
{
    m_vTriangles.emplace_back(triangle);
}

bool CBVHTreeNew::hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const
{
    return hit(ray, t_min, t_max, hit_result, 0u);
}

bool CBVHTreeNew::hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result, uint32_t node_idx) const
{
    auto* node = &m_vNodes[node_idx];
    uint32_t search_stack[64u];
    uint32_t stack_idx{ 0u };

    bool has_hit{ false };
    float closest_hit{ std::numeric_limits<float>::max() };

    while (true)
    {
        if (node->is_leaf())
        {
            for (uint32_t i = 0u; i < node->m_count; i++)
            {
                uint32_t inst_prim = (node_idx << 20u) + m_vIndices[node->m_left + i];
                auto& triangle = m_vTriangles[inst_prim & 0xfffff];
                if (triangle.hit(ray, t_min, closest_hit, hit_result))
                {
                    has_hit = true;
                    closest_hit = hit_result.m_distance;
                }
            }

            if (stack_idx == 0u) 
                break; 
            else 
                node = &m_vNodes[search_stack[--stack_idx]];
            continue;
        }

        uint32_t left_id{ node->m_left };
        uint32_t right_id{ node->m_left + 1u };

        auto dist1 = m_vNodes[left_id].m_aabb.hit(ray, hit_result);
        auto dist2 = m_vNodes[right_id].m_aabb.hit(ray, hit_result);

        if (dist1 > dist2) 
        { 
            std::swap(dist1, dist2); 
            std::swap(left_id, right_id);
        }

        if (math::compare_float(dist1, std::numeric_limits<float>::max()))
        {
            if (stack_idx == 0) 
                break; 
            else 
                node = &m_vNodes[search_stack[--stack_idx]];
        }
        else
        {
            node = &m_vNodes[left_id];
            if (!math::compare_float(dist2, std::numeric_limits<float>::max()))
                search_stack[stack_idx++] = right_id;
        }
    }

    return has_hit;
}

void CBVHTreeNew::build()
{
    m_size = 2ull;

    auto& root = m_vNodes[0ull];
    root.m_left = 0u;
    root.m_count = m_vTriangles.size();

    FAxixAlignedBoundingBox centroid_aabb{};
    grow(0u, centroid_aabb);

    subdivide(0u, 0u, centroid_aabb);
}

void CBVHTreeNew::grow(uint32_t node_idx, FAxixAlignedBoundingBox& aabb)
{
    auto& node = m_vNodes[node_idx];

    node.m_aabb.m_min = aabb.m_min = glm::vec3(std::numeric_limits<float>::max());
    node.m_aabb.m_max = aabb.m_max = glm::vec3(-std::numeric_limits<float>::max());

    for (uint32_t first = node.m_left, i = 0u; i < node.m_count; ++i)
    {
        uint32_t triangle_idx = m_vIndices[first + i];
        auto& triangle = m_vTriangles[triangle_idx];

        auto& triangle_bounds = triangle.bounds();
        node.m_aabb.grow(triangle_bounds);

        auto& centroid = triangle.centroid();
        aabb.m_min = math::min(aabb.m_min, centroid);
        aabb.m_max = math::max(aabb.m_max, centroid);
    }
}

void CBVHTreeNew::subdivide(uint32_t node_idx, uint32_t depth, FAxixAlignedBoundingBox& centroid)
{
    auto& node = m_vNodes[node_idx];

    uint32_t axis{ 0u }, split_pos{ 0u };
    float split_cost = find_best_split(node, axis, split_pos, centroid);

    auto nosplit_cost = node.cost();
    if (math::greater_equal_float(split_cost, nosplit_cost))
        return;

    uint32_t i = node.m_left;
    uint32_t j = i + node.m_count - 1u;

    float scale = static_cast<float>(bins) / (centroid.m_max[axis] - centroid.m_min[axis]);
    while (i <= j)
    {
        auto triangle_idx = m_vIndices[i];
        auto& triangle = m_vTriangles[triangle_idx];

        uint32_t bin_idx = glm::min(bins - 1u, static_cast<uint32_t>((triangle.centroid()[axis] - centroid.m_min[axis]) * scale));
        if (bin_idx < split_pos) 
            i++; 
        else 
            std::swap(m_vIndices[i], m_vIndices[j--]);
    }

    uint32_t left_count = i - node.m_left;
    if (left_count == 0u || left_count == node.m_count)
        return;

    int left_child_idx = m_size++;
    m_vNodes[left_child_idx].m_left = node.m_left;
    m_vNodes[left_child_idx].m_count = left_count;

    int right_chilt_idx = m_size++;
    m_vNodes[right_chilt_idx].m_left = i;
    m_vNodes[right_chilt_idx].m_count = node.m_count - left_count;

    node.m_left = left_child_idx;
    node.m_count = 0u;

    // Subdivide left
    grow(left_child_idx, centroid);
    subdivide(left_child_idx, depth + 1u, centroid);

    // Subdivide right
    grow(right_chilt_idx, centroid);
    subdivide(right_chilt_idx, depth + 1u, centroid);
}

float CBVHTreeNew::find_best_split(FBVHNode& node, uint32_t& axis, uint32_t& split_pos, FAxixAlignedBoundingBox& centroid)
{
    float best_cost = std::numeric_limits<float>::max();
    for (uint32_t a = 0; a < 3u; ++a)
    {
        float boundx_min = centroid.m_min[a];
        float boundx_max = centroid.m_max[a];

        if (math::compare_float(boundx_min, boundx_max))
            continue;

        float scale = static_cast<float>(bins) / (boundx_max - boundx_min);

        std::array<float, bins - 1u> left_area;
        std::array<float, bins - 1u> right_area;

        uint32_t left_sum{ 0u };
        uint32_t right_sum{ 0u };

        std::array<FBVHTreeBin, bins> bin;

        for (uint32_t i = 0u; i < node.m_count; i++)
        {
            auto triangle_idx = m_vIndices[node.m_left + i];
            auto& triangle = m_vTriangles[triangle_idx];
            int binIdx = glm::min(bins - 1u, static_cast<uint32_t>((triangle.centroid()[a] - boundx_min) * scale));
            bin[binIdx].m_count++;
            bin[binIdx].m_bounds.grow(triangle.bounds());
        }

        FAxixAlignedBoundingBox left, right;
        for (uint32_t i = 0u; i < bins - 1u; i++)
        {
            left_sum += bin[i].m_count;
            left.grow(bin[i].m_bounds);
            left_area[i] = static_cast<float>(left_sum) * left.area();

            right_sum += bin[bins - 1u - i].m_count;
            right.grow(bin[bins - 1u - i].m_bounds);
            right_area[bins - 2u - i] = static_cast<float>(right_sum) * right.area();
        }

        scale = (boundx_max - boundx_min) / static_cast<float>(bins);
        for (uint32_t i = 0u; i < bins - 1u; i++)
        {
            const float plane_cost = left_area[i] + right_area[i];
            if (plane_cost < best_cost)
            {
                axis = a;
                split_pos = i + 1u;
                best_cost = plane_cost;
            }
        }
    }

    return best_cost;
}