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
			AddEvent(CreateShared<T>(std::forward<TArgs>(args)...));
		}

		void SetEventCallback(const EventCallback& callback);

		uint32 GetEventCount();
	private:
		void AddEvent(Shared<Event> event);
	private:
		std::queue<Shared<Event>> m_EventQueue;
		EventCallback m_EventCallback;
		std::mutex m_Mutex;
	};

}