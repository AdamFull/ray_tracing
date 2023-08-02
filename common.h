#pragma once

#include <ray_tracer/lib/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <limits>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <filesystem>

using resource_id_t = size_t;
constexpr const resource_id_t invalid_index{ std::numeric_limits<size_t>::max() };