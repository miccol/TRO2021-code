#include <iostream>
#include <thread>
#include "barrier.h"
#include <vector>

#include<chrono>
const int TotalThreads = 2;

//static Barrier barrier(TotalThreads);


class Action
{
 public:
    Action(std::string name, double alpha, Barrier* b) : m_name(name),m_progress(0.0), m_alpha(alpha), m_b(b)
    {
       m_omega = alpha/2.0;

       m_barriers_left.push_back(1.0);
       m_barriers_left.push_back(0.9);
       m_barriers_left.push_back(0.8);
       m_barriers_left.push_back(0.7);
       m_barriers_left.push_back(0.6);
       m_barriers_left.push_back(0.5);
       m_barriers_left.push_back(0.4);
       m_barriers_left.push_back(0.3);
       m_barriers_left.push_back(0.2);
       m_barriers_left.push_back(0.1);

       m_current_barrier = m_barriers_left.back();
       m_barriers_left.pop_back();
    }

    void tick()
    {
     //   std::cout << "Inside Tick " << m_name <<  "current progress "<< m_progress << "current barrier "<< m_current_barrier<<std::endl;

        if(m_progress < m_current_barrier)
        {
            m_progress = m_progress + m_alpha;
        }
        else
        {
        m_b->Wait();
        m_current_barrier = m_barriers_left.back();
        m_barriers_left.pop_back();
        }
    }

   void run()
    {
        while (m_progress < 1.0)
        {
            //std::cout << "ticking" << m_name << std::endl;
            tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

private:
double m_progress;
double m_alpha;
double m_omega;
double m_current_barrier;
std::string m_name;
Barrier* m_b;
std::vector<double> m_barriers_left;
};


static void RunAtSameTime(int index)
{
//    barrier.Wait();   // Comment this line out for Without barrier test.
    std::cout << index << " run at " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
}

int main()
{
    static Barrier barrier(TotalThreads);
    std::thread threads[TotalThreads];
    std::cout << "Start all threads.\n";
    Action action1("action1", 0.02, &barrier);
    Action action2("action2", 0.03, &barrier);


    std::thread t1(&Action::run, action1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::thread t2(&Action::run, action2);
    t1.join();
    t2.join();

}
