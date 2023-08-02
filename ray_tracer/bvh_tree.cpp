#include "bvh_tree.h"

#include "util.h"

#include <iostream>

FAxixAlignedBoundingBox surrounding_box(const FAxixAlignedBoundingBox& lhs, const FAxixAlignedBoundingBox& rhs)
{
    glm::vec3 small{ glm::min(lhs.m_min.x, rhs.m_min.x), glm::min(lhs.m_min.y, rhs.m_min.y), glm::min(lhs.m_min.z, rhs.m_min.z) };
    glm::vec3 big{ glm::max(lhs.m_max.x, rhs.m_max.x), glm::max(lhs.m_max.y, rhs.m_max.y), glm::max(lhs.m_max.z, rhs.m_max.z) };

    return FAxixAlignedBoundingBox(small, big);
}

inline bool box_compare(const std::shared_ptr<CHittable> a, const std::shared_ptr<CHittable> b, int axis)
{
    FAxixAlignedBoundingBox box_a{};
    FAxixAlignedBoundingBox box_b{};

    if (!a->bounds(box_a) || !b->bounds(box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.m_min[axis] < box_b.m_min[axis];
}


bool box_x_compare(const std::shared_ptr<CHittable> a, const std::shared_ptr<CHittable> b) 
{
    return box_compare(a, b, 0);
}

bool box_y_compare(const std::shared_ptr<CHittable> a, const std::shared_ptr<CHittable> b) 
{
    return box_compare(a, b, 1);
}

bool box_z_compare(const std::shared_ptr<CHittable> a, const std::shared_ptr<CHittable> b) 
{
    return box_compare(a, b, 2);
}

CBVHNode::CBVHNode(std::vector<std::shared_ptr<CHittable>>& hittables, size_t start, size_t end)
{
    auto axis = static_cast<int32_t>(random(0.f, 2.f));
    auto comparator = (axis == 0) ? box_x_compare
        : (axis == 1) ? box_y_compare
        : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1ull)
        m_pLeft = m_pRight = hittables[start];
    else if (object_span == 2ull) 
    {
        if (comparator(hittables[start], hittables[start + 1])) 
        {
            m_pLeft = hittables[start];
            m_pRight = hittables[start + 1];
        }
        else 
        {
            m_pLeft = hittables[start + 1];
            m_pRight = hittables[start];
        }
    }
    else 
    {
        std::sort(hittables.begin() + start, hittables.begin() + end, comparator);

        auto mid = start + object_span / 2ull;
        m_pLeft = std::make_shared<CBVHNode>(hittables, start, mid);
        m_pRight = std::make_shared<CBVHNode>(hittables, mid, end);
    }

    FAxixAlignedBoundingBox box_left, box_right;

    if (!m_pLeft->bounds(box_left) || !m_pRight->bounds(box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    m_aabb = box_left.surrounding(box_right);
}

bool CBVHNode::hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const
{
	if (!m_aabb.hit(ray, t_min, t_max))
		return false;

	bool hit_left = m_pLeft->hit(ray, t_min, t_max, hit_result);
	bool hit_right = m_pRight->hit(ray, t_min, hit_left ? hit_result.m_distance : t_max, hit_result);

	return hit_left || hit_right;
}

bool CBVHNode::bounds(FAxixAlignedBoundingBox& output_box) const
{
    output_box = m_aabb;
    return true;
}