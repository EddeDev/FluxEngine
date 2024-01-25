#include "FluxPCH.h"
#include "EventQueue.h"

namespace Flux {

	EventQueue::EventQueue()
	{
	}

	EventQueue::~EventQueue()
	{
	}

	void EventQueue::DispatchEvents()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		while (!m_EventQueue.empty())
		{
			auto& event = m_EventQueue.front();
			m_EventCallback(*event.get());
			m_EventQueue.pop();
		}
	}

	void EventQueue::SetEventCallback(const EventCallback& callback)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_EventCallback = callback;
	}

	uint32 EventQueue::GetEventCount()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return (uint32)m_EventQueue.size();
	}

}