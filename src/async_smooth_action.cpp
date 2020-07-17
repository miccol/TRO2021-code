#include <async_smooth_action.h>
#include <iostream>

using namespace std;

AsyncSmoothAction::AsyncSmoothAction(std::string name, int pause_milliseconds,
                                     double increment)
    : SmoothAction(), CoroActionNode(name, {}),
      pause_milliseconds_(pause_milliseconds), increment_(increment){}

NodeStatus AsyncSmoothAction::tick() {
  while (progress() < 0.9) {
    if (!paused()) {
      set_progress(progress() + increment_);
      cout << "-Stepping " << name() << ".  New Progress " << progress()
           << endl;
      std::this_thread::sleep_for(
          std::chrono::milliseconds(pause_milliseconds_));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return NodeStatus::SUCCESS;
}

void AsyncSmoothAction::pause() {
  set_paused(true);
  cout << "Pausing " << name() << endl;
}

void AsyncSmoothAction::resume() {
  set_paused(false);
  cout << "Resuming " << name() << endl;
}
