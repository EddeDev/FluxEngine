#pragma once

#include "Flux/Runtime/Core/Math/Vector2.h"
#include "Flux/Runtime/Core/Math/Vector3.h"
#include "Flux/Runtime/Core/Math/Vector4.h"
#include "Flux/Runtime/Core/Math/Quaternion.h"

#include <spdlog/fmt/fmt.h>

#define REGISTER_FORMATTER(type, ...) \
template<> \
struct fmt::formatter<type> : fmt::formatter<std::string> \
{ \
	auto format(const type& v, fmt::format_context& context) -> decltype(context.out()) \
	{ \
		return fmt::format_to(context.out(), __VA_ARGS__); \
	} \
}

REGISTER_FORMATTER(Flux::Vector2, "[{0:.2f}, {1:.2f}]", v.X, v.X);
REGISTER_FORMATTER(Flux::Vector3, "[{0:.2f}, {1:.2f}, {2:.2f}]", v.X, v.X, v.X);
REGISTER_FORMATTER(Flux::Vector4, "[{0:.2f}, {1:.2f}, {2:.2f}, {3:.2f}]", v.X, v.Y, v.Z, v.W);
REGISTER_FORMATTER(Flux::Quaternion, "[{0:.2f}, {1:.2f}, {2:.2f}, {3:.2f}]", v.X, v.Y, v.Z, v.W);