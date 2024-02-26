#pragma once

#include <city.h>

namespace Flux {

	struct Guid
	{
	public:
		Guid()
			: m_A(0), m_B(0), m_C(0), m_D(0) {}
		Guid(uint32 a, uint32 b, uint32 c, uint32 d)
			: m_A(a), m_B(b), m_C(c), m_D(d) {}

		explicit Guid(const std::string& guidString)
		{
			if (!Parse(guidString, *this))
				memset(this, 0, sizeof(Guid));
		}

		std::string ToString() const;

		uint32 GetHash() const { return (uint32)CityHash64((char*)this, sizeof(Guid)); }

		bool IsValid() const
		{
			return (m_A | m_B | m_C | m_D) != 0;
		}

		operator bool() const { return IsValid(); }

		friend bool operator==(const Guid& lhs, const Guid& rhs) { return ((lhs.m_A ^ rhs.m_A) | (lhs.m_B ^ rhs.m_B) | (lhs.m_C ^ rhs.m_C) | (lhs.m_D ^ rhs.m_D)) == 0; }
		friend bool operator!=(const Guid& lhs, const Guid& rhs) { return ((lhs.m_A ^ rhs.m_A) | (lhs.m_B ^ rhs.m_B) | (lhs.m_C ^ rhs.m_C) | (lhs.m_D ^ rhs.m_D)) != 0; }
	
		friend bool operator<(const Guid& lhs, const Guid& rhs)
		{
			return  ((lhs.m_A < rhs.m_A) ? true : ((lhs.m_A > rhs.m_A) ? false :
					((lhs.m_B < rhs.m_B) ? true : ((lhs.m_B > rhs.m_B) ? false :
					((lhs.m_C < rhs.m_C) ? true : ((lhs.m_C > rhs.m_C) ? false :
					((lhs.m_D < rhs.m_D) ? true : ((lhs.m_D > rhs.m_D) ? false : false))))))));
		}

		uint32& operator[](int32 index)
		{
			FLUX_ASSERT(index >= 0);
			FLUX_ASSERT(index < 4);

			switch (index)
			{
			case 0: return m_A;
			case 1: return m_B;
			case 2: return m_C;
			case 3: return m_D;
			}
			FLUX_ASSERT(false);
			return m_A;
		}

		const uint32& operator[](int32 index) const
		{
			FLUX_ASSERT(index >= 0);
			FLUX_ASSERT(index < 4);

			switch (index)
			{
			case 0: return m_A;
			case 1: return m_B;
			case 2: return m_C;
			case 3: return m_D;
			}
			FLUX_ASSERT(false);
			return m_A;
		}

		static Guid NewGuid();
		static bool Parse(const std::string& guidString, Guid& outGuid);
	private:
		uint32 m_A;
		uint32 m_B;
		uint32 m_C;
		uint32 m_D;
	};

}

namespace std {

	template <>
	struct hash<Flux::Guid>
	{
		std::size_t operator()(const Flux::Guid& guid) const
		{
			return CityHash64((char*)&guid, sizeof(Flux::Guid));
		}
	};

}