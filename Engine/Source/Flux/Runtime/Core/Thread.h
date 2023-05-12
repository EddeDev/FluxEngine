#pragma once

namespace Flux {

	enum class ThreadPriority : uint8
	{
		None = 0,

		Lowest,
		BelowNormal,
		Normal,
		AboveNormal,
		Highest
	};

	struct ThreadCreateInfo
	{
		std::string Name = "Thread";
		ThreadPriority Priority = ThreadPriority::Normal;
	};

	using ThreadHandle = void*;

	class Thread
	{
	public:
		using Job = std::function<void()>;
	public:
		virtual ~Thread() = default;

		virtual void Join() = 0;
		virtual void Wait() = 0;

		virtual void Submit(Job job) = 0;

		virtual ThreadHandle GetHandle() const = 0;

		static Unique<Thread> Create(const ThreadCreateInfo& createInfo);
	};

}