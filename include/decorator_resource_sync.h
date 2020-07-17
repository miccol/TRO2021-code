#pragma once

#include <behaviortree_cpp_v3/control_node.h>
#include <resource_handler.h>
namespace BT
{
class DecoratorResourceSync : public ControlNode
{
public:
    DecoratorResourceSync(std::string name, ResourceHandler* resource_handler);
    BT::NodeStatus tick();
    void halt();
    double compute_max_priority();
    void addChild(TreeNode *child);

private:
    NodeStatus child_i_status_;
    ResourceHandler* resource_handler_;

};
}
