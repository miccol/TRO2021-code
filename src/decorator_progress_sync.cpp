#include <decorator_progress_sync.h>
#include <smooth_action.h>
#include <barrier.h>
#include <sync_smooth_action.h>
#include <iomanip>      // std::setprecision

using namespace std;


BT::DecoratorProgressSync::DecoratorProgressSync(std::string name, Barrier* barrier_ptr) : ControlNode::ControlNode(name, {}), barrier_ptr_(barrier_ptr)
{
    barrier_ptr->addNode(this);
}

BT::DecoratorProgressSync::DecoratorProgressSync(std::string name) : ControlNode::ControlNode(name, {})
{

}

void DecoratorProgressSync::set_barrier_prt(Barrier* barrier_ptr)
{
    barrier_ptr_ = barrier_ptr;
    barrier_ptr->addNode(this);
}


BT::NodeStatus BT::DecoratorProgressSync::tick()
{
    std::setprecision(5);
    SyncSmoothAction* child_ptr_bt =  (SyncSmoothAction*)children_nodes_[0];

    double child_progress = child_ptr_bt->progress();
    double current_barrier = barrier_ptr_->current_barrier();

    double diff = child_progress - current_barrier;

    child_ptr_bt->set_time(std::chrono::steady_clock::now());

    if (( diff < 0.0001))
    {

        child_ptr_bt->executeTick();
    }

    barrier_ptr_->updateProgress(this, child_progress);

    return NodeStatus::RUNNING;
}


void BT::DecoratorProgressSync::halt()
{

    ControlNode::halt();
}

