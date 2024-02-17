#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"

namespace Flux {

	enum class VertexElementFormat : uint8
	{
		None = 0,
		Float, Float2, Float3, Float4,
		UByte, UByte2, UByte3, UByte4
	};

	namespace Utils {

		static uint32 VertexElementFormatSize(VertexElementFormat format)
		{
			switch (format)
			{
			case VertexElementFormat::Float:  return sizeof(float) * 1;
			case VertexElementFormat::Float2: return sizeof(float) * 2;
			case VertexElementFormat::Float3: return sizeof(float) * 3;
			case VertexElementFormat::Float4: return sizeof(float) * 4;
			case VertexElementFormat::UByte:  return sizeof(uint8) * 1;
			case VertexElementFormat::UByte2: return sizeof(uint8) * 2;
			case VertexElementFormat::UByte3: return sizeof(uint8) * 3;
			case VertexElementFormat::UByte4: return sizeof(uint8) * 4;
			}
			FLUX_VERIFY(false, "Unknown vertex element format!");
			return 0;
		}

		static uint32 VertexElementComponentCount(VertexElementFormat format)
		{
			switch (format)
			{
			case VertexElementFormat::Float:  return 1;
			case VertexElementFormat::Float2: return 2;
			case VertexElementFormat::Float3: return 3;
			case VertexElementFormat::Float4: return 4;
			case VertexElementFormat::UByte:  return 1;
			case VertexElementFormat::UByte2: return 2;
			case VertexElementFormat::UByte3: return 3;
			case VertexElementFormat::UByte4: return 4;
			}
			FLUX_VERIFY(false, "Unknown vertex element format!");
			return 0;
		}

	}

	struct VertexElement
	{
		std::string_view Name;
		VertexElementFormat Format = VertexElementFormat::None;
		uint32 Size = 0;
		uint32 ComponentCount = 0;
		uint32 Offset = 0;
		bool Normalized = false;

		VertexElement() = default;
		VertexElement(std::string_view name, VertexElementFormat format, bool normalized = false)
			: Name(name), Format(format), Normalized(normalized)
		{
			Size = Utils::VertexElementFormatSize(format);
			ComponentCount = Utils::VertexElementComponentCount(format);
		}
	};

	class VertexDeclaration
	{
	public:
		VertexDeclaration() = default;
		VertexDeclaration(const std::initializer_list<VertexElement>& elements)
			: m_Elements(elements)
		{
			uint32 offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		uint32 GetStride() const { return m_Stride; }
		uint32 GetElementCount() const { return static_cast<uint32>(m_Elements.size()); }

		const std::vector<VertexElement>& GetElements() const { return m_Elements; }

		std::vector<VertexElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<VertexElement>::iterator end() { return m_Elements.end(); }
		std::vector<VertexElement>::const_iterator begin() const { return m_Elements.cbegin(); }
		std::vector<VertexElement>::const_iterator end() const { return m_Elements.cend(); }
	private:
		std::vector<VertexElement> m_Elements;

		uint32 m_Stride = 0;
	};

}