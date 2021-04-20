#include "barrier.h"

using namespace std;

Barrier::Barrier(const unsigned initialCount)
    : m_count(initialCount)
{ }

void Barrier::Wait()
{
    unique_lock<mutex> mtxLock(m_mtx);      // Must use unique_lock with condition variable.
    --m_count;
    if (0 == m_count)
    {
        m_cv.notify_all();
    }
    else
    {
        m_cv.wait(mtxLock, [this]{ return 0 == m_count; });      // Blocking till count is zero.
    }
}
