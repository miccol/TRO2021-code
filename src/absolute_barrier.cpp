#include <absolute_barrier.h>
#include <algorithm> // std::reverse
#include <behaviortree_cpp_v3/tree_node.h>
#include <map>
#include <unordered_set>

#define EPSILON 0.000
AbsoluteBarrier::AbsoluteBarrier(std::vector<double> barriers)
    : Barrier(), barriers_(barriers), num_of_barriers_(barriers.size()) {
  std::reverse(barriers_.begin(), barriers_.end());
  current_barrier_ = barriers_.back();
  barriers_.pop_back();
}

void AbsoluteBarrier::updateProgress(BT::TreeNode *node_prt,
                                     double new_progress) {
  tree_nodes_progresses_[node_prt] = new_progress;

  if (new_progress - current_barrier() >= -EPSILON) {
    nodes_at_barrier_.insert(node_prt);
    if (nodes_at_barrier_.size() == num_of_nodes_) {
      current_barrier_ = barriers_.back();
      barriers_.pop_back();
      nodes_at_barrier_.clear();
    }
  }
}
