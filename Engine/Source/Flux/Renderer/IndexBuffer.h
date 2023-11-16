#pragma once

namespace Flux {

	class IndexBuffer : public ReferenceCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref<IndexBuffer> Create(const void* data, uint64 size);
	};

}