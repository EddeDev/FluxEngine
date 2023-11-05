#pragma once

namespace Flux {

	enum class VertexBufferUsage
	{
		Static = 0,
		Dynamic
	};

	class VertexBuffer : public ReferenceCounted
	{
	public:
		virtual ~VertexBuffer() {}

		static Ref<VertexBuffer> Create(const void* data, uint64 size, VertexBufferUsage usage = VertexBufferUsage::Static);
	};

}