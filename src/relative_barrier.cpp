#include <relative_barrier.h>
#include <behaviortree_cpp_v3/tree_node.h>
#include <map>
#include <unordered_set>
#include <algorithm>    // std::reverse
#include <iostream>

#define EPSILON 0.0001

RelativeBarrier::RelativeBarrier(double delta) : Barrier(),
    delta_(delta), min_progress_(1.0),
    min_node_ptr_(nullptr)
{
    current_barrier_ = delta;
}

void RelativeBarrier::updateProgress(BT::TreeNode* node_prt, double new_progress)
{
    tree_nodes_progresses_[node_prt] = new_progress;

    if (new_progress < min_progress_ || (node_prt==min_node_ptr_ && new_progress> min_progress_))
    {
        min_progress_ = new_progress;
        min_node_ptr_ = node_prt;

        setCurrent_barrier(tree_nodes_progresses_[min_node_ptr_] + delta_);
    }
}

double RelativeBarrier::current_barrier() const
{
    return current_barrier_;
}

void RelativeBarrier::setCurrent_barrier(double current_barrier)
{
   // std::cout << "Setting barrier to " <<  current_barrier << std::endl;
    current_barrier_ = current_barrier;
}

void RelativeBarrier::reset()
{
    current_barrier_ = 0.0;
}
