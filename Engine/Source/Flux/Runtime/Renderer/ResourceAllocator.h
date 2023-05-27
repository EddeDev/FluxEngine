#pragma once

namespace Flux {

	enum class ResourceMemoryUsage : uint8
	{
        None = 0,

		GpuOnly,
		CpuOnly,
		CpuToGpu,
		GpuToCpu,
		CpuCopy
	};

	using ResourceAllocation = void*;

	class ResourceAllocator
	{
	public:
		virtual ~ResourceAllocator() = default;
	};

}