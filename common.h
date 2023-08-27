#pragma once

#include <ray_tracer/lib/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

//#define USE_INTRINSICS
//#define USE_INTRINSICS_BASIC_ARITHMETIC
//#define USE_INTRINSICS_OPERATIONS
//#define USE_INTRINSICS_INTERSECTION

#include "random.h"

#include "math.hpp"

//#include "ray_tracer/math/math.hpp"

#include <limits>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <filesystem>
#include <iostream>

#include <algorithm>
#include <numeric>
#include <numbers>
#include <execution>

using resource_id_t = size_t;
constexpr const resource_id_t invalid_index{ std::numeric_limits<size_t>::max() };