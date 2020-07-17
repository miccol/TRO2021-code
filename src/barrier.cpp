#include <barrier.h>
#include <behaviortree_cpp_v3/tree_node.h>
#include <map>

Barrier::Barrier() : num_of_nodes_(0) {}

void Barrier::addNode(BT::TreeNode *node_prt) {
  tree_nodes_progresses_[node_prt] = 0.0;
  num_of_nodes_++;
}

double Barrier::current_barrier() const { return current_barrier_; }

void Barrier::setCurrent_barrier(double current_barrier) {
  current_barrier_ = current_barrier;
}
