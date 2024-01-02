#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

#include "WindowsThread.h"

namespace Flux {

	WindowsThread::WindowsThread(const ThreadCreateInfo& createInfo)
	{
		m_ThreadHandle = CreateThread(NULL, NULL, ThreadProc, this, NULL, &m_ThreadID);

		Platform::SetThreadName(m_ThreadHandle, createInfo.Name.c_str());
		Platform::SetThreadPriority(m_ThreadHandle, createInfo.Priority);

		InitializeCriticalSection(&m_CriticalSection);
		InitializeConditionVariable(&m_ConditionVariable);
	}

	WindowsThread::~WindowsThread()
	{
		Join();
	}

	void WindowsThread::Join()
	{
		Wait();
		Destroy();

		WaitForSingleObject(m_ThreadHandle, INFINITE);
		DeleteCriticalSection(&m_CriticalSection);
	}

	void WindowsThread::Wait()
	{
		EnterCriticalSection(&m_CriticalSection);
		while (!m_Jobs.empty())
		{
			SleepConditionVariableCS(&m_ConditionVariable, &m_CriticalSection, INFINITE);
		}
		LeaveCriticalSection(&m_CriticalSection);
	}

	void WindowsThread::Submit(Job job)
	{
		EnterCriticalSection(&m_CriticalSection);
		m_Jobs.push(std::move(job));
		WakeConditionVariable(&m_ConditionVariable);
		LeaveCriticalSection(&m_CriticalSection);
	}

	uint32 WindowsThread::GetRemainingJobs()
	{
		EnterCriticalSection(&m_CriticalSection);
		uint32 result = static_cast<uint32>(m_Jobs.size());
		WakeConditionVariable(&m_ConditionVariable);
		LeaveCriticalSection(&m_CriticalSection);
		return result;
	}

	void WindowsThread::Destroy()
	{
		EnterCriticalSection(&m_CriticalSection);
		m_Destroying = true;
		WakeConditionVariable(&m_ConditionVariable);
		LeaveCriticalSection(&m_CriticalSection);
	}

	DWORD WINAPI WindowsThread::ThreadProc(LPVOID lpParam)
	{
		WindowsThread& thread = *(WindowsThread*)lpParam;

		while (true)
		{
			EnterCriticalSection(&thread.m_CriticalSection);

			while (thread.m_Jobs.empty() && !thread.m_Destroying)
			{
				SleepConditionVariableCS(&thread.m_ConditionVariable, &thread.m_CriticalSection, INFINITE);
			}

			if (thread.m_Destroying)
			{
				LeaveCriticalSection(&thread.m_CriticalSection);
				break;
			}

			Job& job = thread.m_Jobs.front();

			LeaveCriticalSection(&thread.m_CriticalSection);

			job();

			EnterCriticalSection(&thread.m_CriticalSection);
			thread.m_Jobs.pop();
			WakeConditionVariable(&thread.m_ConditionVariable);
			LeaveCriticalSection(&thread.m_CriticalSection);
		}

		return 0;
	}

}

#endif