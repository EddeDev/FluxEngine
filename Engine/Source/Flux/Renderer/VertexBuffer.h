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

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint64 size, uint64 offset = 0) = 0;

		static Ref<VertexBuffer> Create(uint64 size, VertexBufferUsage usage = VertexBufferUsage::Static);
		static Ref<VertexBuffer> Create(const void* data, uint64 size, VertexBufferUsage usage = VertexBufferUsage::Static);
	};

}