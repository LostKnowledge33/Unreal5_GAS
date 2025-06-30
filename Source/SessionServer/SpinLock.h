#pragma once

#include <atomic>

class SpinLock
{
public:
    void Lock()
    {
        bool expected = false;
        bool desired = true;
        // 다른 누군가 lock을 획득했다면 무한루프.
        while (m_locked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false;
        }
    }
    void UnLock()
    {
        m_locked.store(false);
    }

    bool IsLocked()
	{
		return m_locked.load();
	}

private:
    std::atomic<bool> m_locked = false;
};


class LockManager {
public:
    static SpinLock& getSharedLock() {
        static SpinLock lock;
        return lock;
    }
};