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

}