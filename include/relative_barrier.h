#ifndef RELATIVEBARRIER_H
#define RELATIVEBARRIER_H

#include <barrier.h>
#include <cstddef>

#include <behaviortree_cpp_v3/tree_node.h>
#include <map>
#include <unordered_set>

class RelativeBarrier : public Barrier {
public:
  RelativeBarrier(double delta);

  double currentBarrier() const;
  void setCurrentBarrier(double currentBarrier);

  void reset();
  void updateProgress(BT::TreeNode *node_prt, double new_progress);
  double current_barrier() const;
  void setCurrent_barrier(double current_barrier);

private:
  double delta_;
  double min_progress_;
  BT::TreeNode *min_node_ptr_;

  // std::map<BT::TreeNode *, double> tree_nodes_progresses_;
  std::unordered_set<BT::TreeNode *> nodes_at_barrier_;
};

#endif // BARRIER_H
