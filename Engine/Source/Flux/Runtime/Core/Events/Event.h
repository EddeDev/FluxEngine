#pragma once

#include "EventType.h"

namespace Flux {

	template<typename T>
	using EventCallback = std::function<void(T&)>;

	using EventCallbackID = uint64;

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

}