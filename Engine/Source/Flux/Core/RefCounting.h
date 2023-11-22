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

	class ReferenceCounted
	{
	public:
		virtual ~ReferenceCounted() = default;

		uint32 IncrementReferenceCount() const { return ++m_ReferenceCount; }
		uint32 DecrementReferenceCount() const { return --m_ReferenceCount; }

		uint32 GetReferenceCount() const { return m_ReferenceCount; }

		virtual bool operator==(const ReferenceCounted& other) const { return this == &other; }
		virtual bool operator!=(const ReferenceCounted& other) const { return this != &other; }
	private:
		mutable std::atomic<uint32> m_ReferenceCount;
	};

	template<typename T>
	class Ref
	{
	public:
		Ref()
			: m_Reference(nullptr)
		{
		}

		Ref(std::nullptr_t)
			: m_Reference(nullptr)
		{
		}

		Ref(T* reference)
			: m_Reference(reference)
		{
			IncrementReferenceCount();
		}

		Ref(const Ref<T>& other)
			: m_Reference(other.m_Reference)
		{
			IncrementReferenceCount();
		}

		template<typename TOther>
		Ref(const Ref<TOther>& other)
			: m_Reference(static_cast<T*>(other.m_Reference))
		{
			IncrementReferenceCount();
		}

		template<typename TOther>
		Ref(Ref<TOther>&& other)
			: m_Reference(static_cast<T*>(other.m_Reference))
		{
			other.m_Reference = nullptr;
		}

		~Ref()
		{
			DecrementReferenceCount();
		}

		Ref& operator=(std::nullptr_t)
		{
			DecrementReferenceCount();
			m_Reference = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other)
		{
			other.IncrementReferenceCount();
			DecrementReferenceCount();
			m_Reference = other.m_Reference;
			return *this;
		}

		template<typename TOther>
		Ref& operator=(const Ref<TOther>& other)
		{
			other.IncrementReferenceCount();
			DecrementReferenceCount();
			m_Reference = other.m_Reference;
			return *this;
		}

		template<typename TOther>
		Ref& operator=(Ref<TOther>&& other)
		{
			DecrementReferenceCount();
			m_Reference = other.m_Reference;
			other.m_Reference = nullptr;
			return *this;
		}

		bool operator==(const Ref<T>& other) const { return m_Reference == other.m_Reference; }
		bool operator!=(const Ref<T>& other) const { return !(*this == other); }

		operator bool() { return m_Reference != nullptr; }
		operator bool() const { return m_Reference != nullptr; }

		T* operator->() { return m_Reference; }
		const T* operator->() const { return m_Reference; }

		T& operator*() { return *m_Reference; }
		const T& operator*() const { return *m_Reference; }

		T* Get() { return m_Reference; }
		const T* Get() const { return m_Reference; }

		bool Equals(const Ref<T>& other)
		{
			if (!m_Reference || !other.m_Reference)
				return false;
			return *m_Reference == *other.m_Reference;
		}

		template<typename TOther>
		Ref<TOther> As() const
		{
			return Ref<TOther>(*this);
		}

		template<typename... TArgs>
		static Ref<T> Create(TArgs&&... args)
		{
			return Ref<T>(new T(std::forward<TArgs>(args)...));
		}
	private:
		void IncrementReferenceCount() const
		{
			if (m_Reference)
				m_Reference->IncrementReferenceCount();
		}

		void DecrementReferenceCount() const
		{
			if (m_Reference)
			{
				if (m_Reference->DecrementReferenceCount() == 0)
				{
					delete m_Reference;
					m_Reference = nullptr;
				}
			}
		}
	private:
		mutable T* m_Reference;

		template<class TOther>
		friend class Ref;
	};

}