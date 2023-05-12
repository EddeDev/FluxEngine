#pragma once

#include <spdlog/fmt/fmt.h>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#define REGISTER_FORMATTER(type, ...) \
template<> \
struct fmt::formatter<type> : fmt::formatter<std::string> \
{ \
	auto format(const type& v, fmt::format_context& context) -> decltype(context.out()) \
	{ \
		return fmt::format_to(context.out(), __VA_ARGS__); \
	} \
}

REGISTER_FORMATTER(glm::fvec2, "[{0:.2f}, {1:.2f}]", v.x, v.y);
REGISTER_FORMATTER(glm::fvec3, "[{0:.2f}, {1:.2f}, {2:.2f}]", v.x, v.y, v.z);
REGISTER_FORMATTER(glm::fvec4, "[{0:.2f}, {1:.2f}, {2:.2f}, {3:.2f}]", v.x, v.y, v.z, v.w);

REGISTER_FORMATTER(glm::dvec2, "[{0:.2}, {1:.2}]", v.x, v.y);
REGISTER_FORMATTER(glm::dvec3, "[{0:.2}, {1:.2}, {2:.2}]", v.x, v.y, v.z);
REGISTER_FORMATTER(glm::dvec4, "[{0:.2}, {1:.2}, {2:.2}, {3:.2}]", v.x, v.y, v.z, v.w);

REGISTER_FORMATTER(glm::ivec2, "[{0}, {1}]", v.x, v.y);
REGISTER_FORMATTER(glm::ivec3, "[{0}, {1}, {2}]", v.x, v.y, v.z);
REGISTER_FORMATTER(glm::ivec4, "[{0}, {1}, {2}, {3}]", v.x, v.y, v.z, v.w);

REGISTER_FORMATTER(glm::bvec2, "[{0}, {1}]", v.x, v.y);
REGISTER_FORMATTER(glm::bvec3, "[{0}, {1}, {2}]", v.x, v.y, v.z);
REGISTER_FORMATTER(glm::bvec4, "[{0}, {1}, {2}, {3}]", v.x, v.y, v.z, v.w);

REGISTER_FORMATTER(glm::mat2, "\n{0}\n{1}", v[0], v[1]);
REGISTER_FORMATTER(glm::mat3, "\n{0}\n{1}\n{2}", v[0], v[1], v[2]);

template<>
struct fmt::formatter<glm::mat4> : fmt::formatter<std::string>
{
	auto format(const glm::mat4& v, fmt::format_context& context) -> decltype(context.out())
	{
		glm::vec3 translation, scale, skew;
		glm::quat orientation;
		glm::vec4 perspective;
		if (!glm::decompose(v, scale, orientation, translation, skew, perspective))
			return fmt::format_to(context.out(), "\n{0}\n{1}\n{2}\n{3}", v[0], v[1], v[2], v[3]);

		glm::vec3 rotation = glm::degrees(glm::eulerAngles(orientation));
		return fmt::format_to(context.out(), "\nTranslation: {0}\nRotation: {1}\nScale: {2}", translation, rotation, scale);
	}
};