#pragma once

#include <mutex>
#include <condition_variable>

class Barrier
{
private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    unsigned m_count;

public:
    /// <summary>Ctor. Initializes a Barrier with count.</summary>
    explicit Barrier(const unsigned initialCount);

    /// <summary>
    /// Block the calling thread until the internal count reaches the value zero. Then all waiting threads are unblocked.
    /// </summary>
    void Wait();
};
