#pragma once

#include "Event.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	class EventManager
	{
	public:
		EventManager();
		~EventManager();

		template<typename T>
		EventCallbackID Subscribe(const EventCallback<T>& callback)
		{
			static_assert(std::is_base_of<Event, T>::value);

			EventCallbackID callbackID = static_cast<EventCallbackID>(callback.target_type().hash_code());

			EventType type = T::GetStaticType();
			if (type == EventType::None)
			{
				FLUX_VERIFY(false);
				return callbackID;
			}

			std::lock_guard<std::mutex> lock(m_EventMutex);

			if (m_EventCallbacks.find(callbackID) != m_EventCallbacks.end())
			{
				FLUX_VERIFY(false);
				return callbackID;
			}
		
			auto& eventIDs = m_EventIDs[type];
			if (std::find(eventIDs.begin(), eventIDs.end(), callbackID) != eventIDs.end())
			{
				FLUX_VERIFY(false);
				return callbackID;
			}

			m_EventIDs[type].push_back(callbackID);
			
			m_EventCallbacks[callbackID] = [callback](Event& e)
			{
				callback(*(T*)&e);
			};

			return callbackID;
		}

		void Unsubscribe(EventCallbackID callbackID)
		{
			std::lock_guard<std::mutex> lock(m_EventMutex);

			auto it = m_EventCallbacks.find(callbackID);
			if (it == m_EventCallbacks.end())
			{
				FLUX_VERIFY(false);
				return;
			}
			
			m_EventCallbacks.erase(it);
		}

		template<typename T, typename... TArgs>
		void SubmitEvent(TArgs&&... args)
		{
			static_assert(std::is_base_of<Event, T>::value);

			EventType type = T::GetStaticType();
			FLUX_ASSERT(type != EventType::None);

			auto event = T(std::forward<TArgs>(args)...);

			FLUX_TRACE("Submitting Event: {0}", Utils::EventTypeToString(type));

			std::lock_guard<std::mutex> lock(m_EventMutex);

			auto it = m_EventIDs.find(type);
			if (it == m_EventIDs.end())
				return;
			
			for (auto& callbackID : it->second)
			{
				auto callbackIt = m_EventCallbacks.find(callbackID);
				FLUX_ASSERT(callbackIt != m_EventCallbacks.end());

				auto& func = callbackIt->second;
				FLUX_ASSERT(func);

				func(event);
			}
		}
	private:
		std::unordered_map<EventCallbackID, std::function<void(Event&)>> m_EventCallbacks;
		std::unordered_map<EventType, std::vector<EventCallbackID>> m_EventIDs;
		std::mutex m_EventMutex;
	};

}