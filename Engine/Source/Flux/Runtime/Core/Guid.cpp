#include "FluxPCH.h"
#include "Guid.h"

namespace Flux {

	namespace Utils {

		static int32 HexDigit(char c)
		{
			int32 result;
			if (c >= '0' && c <= '9')
				result = c - '0';
			else if (c >= 'a' && c <= 'f')
				result = c + 10 - 'a';
			else if (c >= 'A' && c <= 'F')
				result = c + 10 - 'A';
			else
				result = 0;
			return result;
		}

		static uint32 HexNumber(const char* hexString)
		{
			uint32 result = 0;
			while (*hexString)
			{
				result *= 16;
				result += HexDigit(*hexString++);
			}
			return result;
		}

	}

	std::string Guid::ToString() const
	{
		return fmt::format("{0:x}{1:x}{2:x}{3:x}", m_A, m_B, m_C, m_D);
	}

	Guid Guid::NewGuid()
	{
		Guid result;
#ifdef FLUX_PLATFORM_WINDOWS
	#ifdef FLUX_ENABLE_ASSERTS
			FLUX_ASSERT(CoCreateGuid((GUID*)&result) == S_OK);
	#else
			CoCreateGuid((GUID*)&result);
	#endif
#else
	#error Unknown platform
#endif
		return result;
	}

	bool Guid::Parse(const std::string& guidString, Guid& outGuid)
	{
		size_t offset = 0;
		for (uint32 index = 0; index < 4; index++)
		{
			outGuid[index] = Utils::HexNumber(guidString.substr(offset, 8).c_str());
			offset += 8;
		}
		return true;
	}

}