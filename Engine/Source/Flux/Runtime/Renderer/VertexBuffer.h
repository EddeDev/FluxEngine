#pragma once

namespace Flux {

	enum class VertexBufferUsage : uint8
	{
		Static = 0,
		Dynamic = 1,
		Stream = 2
	};

	class VertexBuffer : public ReferenceCounted
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint64 size, uint64 offset = 0) = 0;

		virtual uint64 GetSize() const = 0;

		virtual VertexBufferUsage GetUsage() const = 0;

		static Ref<VertexBuffer> Create(uint64 size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		static Ref<VertexBuffer> Create(const void* data, uint64 size, VertexBufferUsage usage = VertexBufferUsage::Static);
	};

}
