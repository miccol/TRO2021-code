#ifndef ABSOLUTEBARRIER_H
#define ABSOLUTEBARRIER_H

#include <barrier.h>

#include <behaviortree_cpp_v3/tree_node.h>
#include <unordered_set>
class AbsoluteBarrier : public Barrier {
public:
  AbsoluteBarrier(std::vector<double> barriers);

  // void reset();
  void updateProgress(BT::TreeNode *node_prt, double new_progress) override;

private:
  std::vector<double> barriers_;
  std::size_t num_of_barriers_;

  double increment_;

  std::unordered_set<BT::TreeNode *> nodes_at_barrier_;
};

#endif // ABSOLUTEBARRIER_H
