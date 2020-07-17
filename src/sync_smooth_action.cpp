#include <iostream>
#include <sync_smooth_action.h>
#include <iomanip>      // std::setprecision
#include <chrono>
using namespace std;

SyncSmoothAction::SyncSmoothAction(std::string name, int pause_milliseconds, double increment) : SmoothAction(), SyncActionNode(name, {}), pause_milliseconds_(pause_milliseconds), increment_(increment) {}
SyncSmoothAction::~SyncSmoothAction() {}

NodeStatus SyncSmoothAction::tick()
{

    std::setprecision(5);

    if(!paused())
    {
        set_progress(min(progress() + increment_, 1.0));
    }
    cout << "Action " << name() << " Progress: " << progress() <<endl;
    std::this_thread::sleep_for (std::chrono::milliseconds(pause_milliseconds_));

    return NodeStatus::SUCCESS;
}

void SyncSmoothAction::pause()
{
    set_paused(true);
    cout<< "Pausing " << name() << endl;
}

void SyncSmoothAction::resume()
{
    set_paused(false);
    cout<< "Resuming " << name() << endl;
}


