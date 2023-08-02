#pragma once

#include "hittable.h"

class CBVHNode : public CHittable
{
public:
	CBVHNode() = default;
	CBVHNode(std::vector<std::shared_ptr<CHittable>>& hittables, size_t start, size_t end);

	bool hit(const FRay& r, float t_min, float t_max, FHitResult& hit_result) const override;
	bool bounds(FAxixAlignedBoundingBox& output_box) const override;
private:
	std::shared_ptr<CHittable> m_pLeft{};
	std::shared_ptr<CHittable> m_pRight{};
};