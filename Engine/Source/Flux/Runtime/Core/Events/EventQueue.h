#pragma once

#include "Event.h"

namespace Flux {

	using EventCallback = std::function<void(Event&)>;

	class EventQueue : public ReferenceCounted
	{
	public:
		EventQueue();
		virtual ~EventQueue();

		void DispatchEvents();

		template<typename T, typename... TArgs>
		void AddEvent(TArgs&&... args)
		{
			static_assert(std::is_base_of<Event, T>::value);

			Shared<Event> event = CreateShared<T>(std::forward<TArgs>(args)...);

			std::lock_guard<std::mutex> lock(m_Mutex);
			m_EventQueue.push(event);
		}

		void SetEventCallback(const EventCallback& callback);

		uint32 GetEventCount();
	private:
		std::queue<Shared<Event>> m_EventQueue;
		EventCallback m_EventCallback;
		std::mutex m_Mutex;
	};

}