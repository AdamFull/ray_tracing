#include "components.h"

void FMeshPrimitive::set_dimensions(const glm::vec3& min, const glm::vec3& max)
{
	dimensions.min = min;
	dimensions.max = max;
	dimensions.size = max - min;
	dimensions.center = (min + max) / 2.0f;
	dimensions.radius = glm::distance(min, max) / 2.0f;
}