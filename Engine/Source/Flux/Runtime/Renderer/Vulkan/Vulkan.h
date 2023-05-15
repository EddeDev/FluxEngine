#pragma once

#include <vulkan/vulkan.h>

#ifdef FLUX_PLATFORM_WINDOWS
	#include <vulkan/vulkan_win32.h>
#endif

namespace Flux {

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_VK_CHECKS_ENABLED
#endif

#ifdef FLUX_VK_CHECKS_ENABLED
	#define VK_CHECK(result) if (result != VK_SUCCESS) FLUX_VERIFY(false)
#else
	#define VK_CHECK(result) result
#endif

}