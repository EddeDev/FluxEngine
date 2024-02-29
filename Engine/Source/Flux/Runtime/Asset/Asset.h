#pragma once

#include "AssetType.h"
#include "AssetID.h"

#include "Flux/Runtime/Core/RefCounting.h"

namespace Flux {

#define ASSET_CLASS_TYPE(type) \
		static AssetType GetStaticType() { return AssetType::##type; } \
		virtual AssetType GetType() const override { return GetStaticType(); }

	class Asset : public ReferenceCounted
	{
	public:
		virtual ~Asset() {}

		void SetAssetID(const AssetID& assetID) { m_AssetID = assetID; }
		const AssetID& GetAssetID() const { return m_AssetID; }

		virtual AssetType GetType() const = 0;
	private:
		AssetID m_AssetID;
	};

}