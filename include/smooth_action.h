#ifndef SMOOTHACTION_H
#define SMOOTHACTION_H

#include <behaviortree_cpp_v3/behavior_tree.h>
#include <chrono>
#include <string>
using namespace std;

namespace BT {

class SmoothAction {
public:
  // Constructor
  SmoothAction();
  ~SmoothAction();

  // Methods used to access the node state without the
  // conditional waiting (only mutual access)

  // The method that is going to be executed by the thread
  virtual BT::NodeStatus tick() = 0;

  // The method used to interrupt the execution of the node
  virtual void pause() = 0;
  virtual void resume() = 0;
  double progress() const;
  void set_progress(const double progress);

  bool paused() const;
  void set_paused(bool paused);

  void allocateResource(const string resource);
  void releaseResource(const string resource);

  std::vector<string> resources_allocated() const;

  void allocateResources(const std::vector<string> resources);
  void releaseResources(const std::vector<string> resources);
  void set_time(std::chrono::steady_clock::time_point time);

  std::chrono::steady_clock::time_point time() const;

private:
  double progress_;
  std::chrono::steady_clock::time_point time_;
  bool paused_;
  std::vector<string> resources_allocated_;
};
} // namespace BT

#endif // SMOOTHACTION_H
