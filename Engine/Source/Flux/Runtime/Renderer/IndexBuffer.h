#pragma once

namespace Flux {

	enum class IndexBufferDataType : uint8
	{
		UInt32 = 0,
		UInt16,
		UInt8,
	};

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

		virtual IndexBufferDataType GetDataType() const = 0;
		virtual IndexBufferUsage GetUsage() const = 0;

		static Ref<IndexBuffer> Create(uint64 size, IndexBufferDataType dataType, IndexBufferUsage usage = IndexBufferUsage::Static);
		static Ref<IndexBuffer> Create(const void* data, uint64 size, IndexBufferDataType dataType, IndexBufferUsage usage = IndexBufferUsage::Static);
	};

	namespace Utils {

		inline static uint32 IndexBufferDataTypeSize(IndexBufferDataType dataType)
		{
			switch (dataType)
			{
			case IndexBufferDataType::UInt32: return sizeof(uint32);
			case IndexBufferDataType::UInt16: return sizeof(uint16);
			case IndexBufferDataType::UInt8:  return sizeof(uint8);
			}
			FLUX_VERIFY(false, "Unknown index buffer data type!");
			return 0;
		}

	}

}