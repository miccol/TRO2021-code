#ifndef ASYNC_SMOOTH_ACTION_H
#define ASYNC_SMOOTH_ACTION_H

#include <smooth_action.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

using namespace BT;
class AsyncSmoothAction : public SmoothAction, public CoroActionNode
{
private:
    int pause_milliseconds_;
    double increment_;
public:
    AsyncSmoothAction(std::string name, int pause_milliseconds = 0, double increment = 0.1);

    NodeStatus tick() override;

    void pause() override;

    void resume() override;
};

#endif // ASYNC_SMOOTH_ACTION_H
