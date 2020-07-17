#include <decorator_resource_sync.h>
#include <smooth_action.h>
#include <barrier.h>
#include <sync_smooth_action.h>
#include <string>
using namespace std;


BT::DecoratorResourceSync::DecoratorResourceSync(std::string name, ResourceHandler *resource_handler) : ControlNode::ControlNode(name, {}), resource_handler_(resource_handler)
{
}

BT::NodeStatus BT::DecoratorResourceSync::tick()
{

    SyncSmoothAction* child_ptr_bt =  (SyncSmoothAction*)children_nodes_[0];

    vector<string> resources_used = child_ptr_bt->resources_allocated();

    bool is_ok = resource_handler_->areResourcesAvailable(this, resources_used);

    NodeStatus status = NodeStatus::RUNNING;
    if (is_ok)
    {
        if ( resource_handler_->priorityOf(child_ptr_bt)>= resource_handler_->max_priority())
        {
        resource_handler_->allocateResource(this, resources_used);
        child_ptr_bt->executeTick();
        }
        else
        {
            //preemption to avoid starvation
            resource_handler_->releaseResources(resources_used);
        }
    }
    else
    {
       resource_handler_->increasePritority(child_ptr_bt);

    }

    return status;
}

void BT::DecoratorResourceSync::halt()
{
    ControlNode::halt();
}




void DecoratorResourceSync::addChild(TreeNode *child)
{
    ControlNode::addChild(child);
}


