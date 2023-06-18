#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"
#include "Flux/Runtime/Core/Logging/LogMacros.h"
#include "Flux/Runtime/Core/AssertionMacros.h"

#include <chrono>

namespace Flux {

	enum class TimeUnit : uint8
	{
		Seconds = 0,
		Milliseconds
	};

	class Timer
	{
	public:
		Timer(TimeUnit unit = TimeUnit::Seconds)
			: m_Unit(unit)
		{
			Reset();
		}

		void Reset()
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
		}

		float GetTime()
		{
			switch (m_Unit)
			{
			case TimeUnit::Seconds: return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count() * 0.001f * 0.001f;
			case TimeUnit::Milliseconds: return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count() * 0.001f;
			}

			FLUX_VERIFY(false, "Unknown time unit");
			return 0.0f;
		}
	private:
		TimeUnit m_Unit;

		std::chrono::high_resolution_clock::time_point m_StartTime;
		std::chrono::high_resolution_clock::time_point m_StopTime;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(std::string_view title, TimeUnit unit = TimeUnit::Seconds)
			: m_Title(title), m_Unit(unit)
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
		}

		~ScopedTimer()
		{
			m_StopTime = std::chrono::high_resolution_clock::now();

			switch (m_Unit)
			{
			case TimeUnit::Seconds:
			{
				auto duration = std::chrono::duration_cast<std::chrono::seconds>(m_StopTime - m_StartTime);
				FLUX_TRACE_CATEGORY("Timer", "{0} took {1} seconds", m_Title, duration.count());
				break;
			}
			case TimeUnit::Milliseconds:
			{
				FLUX_TRACE_CATEGORY("Timer", "{0} took {1}ms", m_Title, std::chrono::duration_cast<std::chrono::microseconds>(m_StopTime - m_StartTime).count() * 0.001f);
				break;
			}
			}
		}
	private:
		std::string_view m_Title;
		TimeUnit m_Unit;

		std::chrono::high_resolution_clock::time_point m_StartTime;
		std::chrono::high_resolution_clock::time_point m_StopTime;
	};

// TODO: move to separate file
#define FLUX_TOKEN_PASTE(prefix, suffix) prefix##suffix
#define FLUX_CONCAT(prefix, suffix) FLUX_TOKEN_PASTE(prefix, suffix)

#define FLUX_SCOPED_TIMER(title, unit) \
	FLUX_CONCAT(ScopedTimer scopedTimer, __LINE__)(title, TimeUnit::unit);

}