#pragma once

#include "AssetType.h"

#include "Flux/Runtime/Core/Guid.h"
#include "Flux/Runtime/Core/RefCounting.h"

namespace Flux {

#define ASSET_CLASS_TYPE(type) \
		static AssetType GetStaticType() { return AssetType::##type; } \
		virtual AssetType GetType() const override { return GetStaticType(); }

	class Asset : public ReferenceCounted
	{
	public:
		virtual ~Asset() {}

		void SetID(const Guid& id) { m_ID = id; }
		const Guid& GetID() const { return m_ID; }

		virtual AssetType GetType() const = 0;
	private:
		Guid m_ID;
	};

}