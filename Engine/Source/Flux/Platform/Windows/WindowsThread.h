#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Core/Thread.h"

namespace Flux {

	class WindowsThread : public Thread
	{
	public:
		WindowsThread(const ThreadCreateInfo& createInfo);
		virtual ~WindowsThread();

		virtual void Join() override;
		virtual void Wait() override;

		virtual void Submit(Job job) override;
		virtual uint32 GetRemainingJobs() override;

		virtual ThreadHandle GetHandle() const override { return m_ThreadHandle; }
		virtual ThreadID GetID() const override { return static_cast<ThreadID>(m_ThreadID); }
	private:
		void Destroy();

		static DWORD WINAPI ThreadProc(LPVOID lpParam);
	private:
		HANDLE m_ThreadHandle = NULL;
		DWORD m_ThreadID = NULL;

		CRITICAL_SECTION m_CriticalSection;
		CONDITION_VARIABLE m_ConditionVariable;

		std::queue<Job> m_Jobs;
		bool m_Destroying = false;
	};

}

#endif