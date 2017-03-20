#ifndef __spinlock_mutex__
#define __spinlock_mutex__

#include <atomic>

using namespace std;

class spinlock_mutex
{
	std::atomic_flag flag;

public:
	spinlock_mutex() //: flag(ATOMIC_FLAG_INIT) // (ATOMIC_FLAG_INIT)*/
	{
		// std::atomic_flag flag = ATOMIC_FLAG_INIT;
		// ATOMIC_FLAG_INIT(flag);
		unlock();
	}

	void lock()
	{
		while(flag.test_and_set(std::memory_order_acquire));
	}
	void unlock()
	{
		flag.clear(std::memory_order_release);
	}
};
#endif