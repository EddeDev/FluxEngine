#pragma once

namespace Flux {

	template<typename T>
	using Unique = std::unique_ptr<T>;

	template<typename T>
	using Shared = std::shared_ptr<T>;

	template<typename T, typename... TArgs>
	constexpr Unique<T> CreateUnique(TArgs&&... args)
	{
		return std::make_unique<T>(std::forward<TArgs>(args)...);
	}

	template<typename T, typename... TArgs>
	constexpr Shared<T> CreateShared(TArgs&&... args)
	{
		return std::make_shared<T>(std::forward<TArgs>(args)...);
	}

}