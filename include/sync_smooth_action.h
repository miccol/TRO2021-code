#ifndef SYNC_SMOOTH_ACTION_H
#define SYNC_SMOOTH_ACTION_H

#include <smooth_action.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

using namespace BT;
class SyncSmoothAction : public SmoothAction, public SyncActionNode
{
private:
    int pause_milliseconds_;
    double increment_;
public:
    SyncSmoothAction(std::string name, int pause_milliseconds = 0, double increment = 0.1);

    NodeStatus tick() override;

    void pause() override;

    void resume() override;

    ~SyncSmoothAction();
};


#endif // SYNC_SMOOTH_ACTION_H
