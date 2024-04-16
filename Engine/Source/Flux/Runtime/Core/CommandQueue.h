#pragma once

#include "Buffer.h"

namespace Flux {

	class CommandQueue
	{
	private:
		typedef void (*CommandFn)(void*);
	public:
		CommandQueue(const std::string& debugName, uint64_t initialSize);
		~CommandQueue();

		template<typename TFunc>
		void Push(TFunc&& func)
		{
			auto buffer = Allocate(Execute<TFunc>, sizeof(TFunc));
			new (buffer) TFunc(std::forward<TFunc>(func));
		}

		void Flush();
	private:
		void* Allocate(CommandFn func, uint32_t size);
		void Resize(uint32_t currentOffset, uint32_t newCapacity);

		template<typename TFunc>
		static void Execute(void* pointer)
		{
			TFunc* command = (TFunc*)pointer;
			(*command)();
			command->~TFunc();
		}
	private:
		Buffer m_Buffer;
		uint8_t* m_BufferPointer;
		std::string m_DebugName;
		bool m_ShouldShrink = false;
	};

}