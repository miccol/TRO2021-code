#ifndef RESOURCE_HANDLER_H
#define RESOURCE_HANDLER_H

#include <behaviortree_cpp_v3/tree_node.h>
#include <map>
#include <mutex>
#include <string>
using namespace std;
using namespace BT;

typedef double (*incFuncType)(double);

class ResourceHandler {
public:
  ResourceHandler(incFuncType iFunction = [](double x) { return x; });
  bool allocateResource(TreeNode *tree_node_ptr,
                        const vector<string> resources);
  bool releaseResource(const string resource);
  // bool isResourceAllocated(const string resource) const;
  bool releaseResources(const vector<string> resources);
  bool areResourcesAvailable(TreeNode *tree_node_ptr,
                             const vector<string> resources);

  double max_priority();
  void increasePritority(BT::TreeNode *node);
  void addNode(BT::TreeNode *node);
  double priorityOf(BT::TreeNode *child);

private:
  mutable map<string, bool> resources_allocated_;
  mutable std::mutex resources_mutex; // protects resources_allocated_
  std::map<string, BT::TreeNode *> nodes_allocating_;
  std::map<BT::TreeNode *, double> nodes_priorities_;
  incFuncType incrementFunction;
};

#endif // RESOURCE_HANDLER_H
