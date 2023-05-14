#pragma once

#include <vulkan/vulkan.h>

namespace Flux {

#define VK_CHECK(result) if (result != VK_SUCCESS) FLUX_VERIFY(false)

}