#pragma once

#include <behaviortree_cpp_v3/control_node.h>
#include <barrier.h>
namespace BT
{
class DecoratorProgressSync : public ControlNode
{
public:
    DecoratorProgressSync(std::string name, Barrier *barrier_ptr);
    DecoratorProgressSync(std::string name);
    BT::NodeStatus tick();
    void halt();

    void set_barrier_prt(Barrier *barrier_ptr);
private:
    Barrier* barrier_ptr_;
    NodeStatus child_i_status_;

};
}
