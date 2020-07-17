#ifndef BARRIER_H
#define BARRIER_H

#include <behaviortree_cpp_v3/tree_node.h>
#include <map>

class Barrier {

public:
  Barrier();
  void addNode(BT::TreeNode *node_prt);
  virtual void updateProgress(BT::TreeNode *node_prt, double new_progress) = 0;
  double current_barrier() const;
  void setCurrent_barrier(double current_barrier);

protected:
  std::size_t num_of_nodes_;
  double current_barrier_;
  std::map<BT::TreeNode *, double> tree_nodes_progresses_;
};

#endif // BARRIER_H
