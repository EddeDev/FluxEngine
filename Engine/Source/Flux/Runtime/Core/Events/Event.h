#pragma once

#include "EventType.h"

namespace Flux {

#define EVENT_CLASS_TYPE(type) \
		static EventType GetStaticType() { return EventType::##type; } \
		virtual EventType GetType() const override { return GetStaticType(); }

	class Event
	{
	public:
		virtual EventType GetType() const = 0;

		void SetHandled(bool handled) { m_Handled = handled; }
		bool IsHandled() const { return m_Handled; }
	private:
		bool m_Handled = false;
	};

	using EventCallback = std::function<void(Event&)>;

	class EventHandler
	{
	public:
		EventHandler(Event& event)
			: m_Event(event) {}

		template<typename T, typename TFunc>
		void Bind(TFunc func)
		{
			if (m_Event.GetType() == T::GetStaticType())
				func(*(T*)&m_Event);
		}
	private:
		Event& m_Event;
	};

	class EventQueue : public ReferenceCounted
	{
	public:
		EventQueue() {}
		virtual ~EventQueue() {}

		void DispatchEvents()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			while (!m_EventQueue.empty())
			{
				auto& event = m_EventQueue.front();
				// std::cout << "Event: " << Utils::EventTypeToString(event->GetType()) << std::endl;
				m_EventCallback(*event.get());
				m_EventQueue.pop();
			}
		}

		template<typename T,  typename... TArgs>
		void AddEvent(TArgs&&... args)
		{
			static_assert(std::is_base_of<Event, T>::value);

			Shared<Event> event = CreateShared<T>(std::forward<TArgs>(args)...);

			std::lock_guard<std::mutex> lock(m_Mutex);
			m_EventQueue.push(event);
		}

		void SetEventCallback(const EventCallback& callback)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);

			m_EventCallback = callback;
		}
	private:
		std::queue<Shared<Event>> m_EventQueue;
		EventCallback m_EventCallback;
		std::mutex m_Mutex;
	};

}