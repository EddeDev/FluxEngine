#pragma once

namespace Flux {

	enum class IndexBufferUsage : uint8
	{
		Static = 0,
		Dynamic,
		Stream
	};

	class IndexBuffer : public ReferenceCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint64 size, uint64 offset = 0) = 0;

		virtual uint64 GetSize() const = 0;

		virtual IndexBufferUsage GetUsage() const = 0;

		static Ref<IndexBuffer> Create(uint64 size, IndexBufferUsage usage = IndexBufferUsage::Static);
		static Ref<IndexBuffer> Create(const void* data, uint64 size, IndexBufferUsage usage = IndexBufferUsage::Static);
	};

}