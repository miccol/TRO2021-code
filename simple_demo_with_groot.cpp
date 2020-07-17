/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file simple_demo_with_groot.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */

#ifdef ZMQ_FOUND
#include "behaviortree_cpp_v3/loggers/bt_zmq_publisher.h"
#endif

#include <behaviortree_cpp_v3/behavior_tree.h>
#include <behaviortree_cpp_v3/bt_factory.h>
#include <chrono> // std::chrono::seconds
#include <iostream>
#include <thread> // std::this_thread::sleep_for

#include <absolute_barrier.h>
#include <decorator_progress_sync.h>
#include <sync_smooth_action.h>

using namespace BT;
using namespace std;

void run_from_groot() {
  // 1 Set the custom made nodes for decorator
  BehaviorTreeFactory bt_factory;
  bt_factory.registerNodeType<DecoratorProgressSync>("AbsoluteProgressSync");
  bt_factory.registerNodeType<SyncSmoothAction>("SmoothAction");
  // 2 Load BT from file
  BT::Tree tree = bt_factory.createTreeFromFile("./abs_progress_example.xml");

  AbsoluteBarrier barrier({0.0, 0.2, 0.4, 0.5, 0.7, 0.8, 0.9, 1.0});

  vector<TreeNode::Ptr> all_nodes_prt = tree.nodes;

  for (TreeNode::Ptr node : all_nodes_prt) {
    if (node->name() == "AbsoluteProgressSync") {
      std::dynamic_pointer_cast<DecoratorProgressSync>(node)->set_barrier_prt(
          &barrier);
    }
  }
  // 3 Connect BT to groot visualizer
#ifdef ZMQ_FOUND
  PublisherZMQ publisher_zmq(tree);
#endif

  // 3 Execute the BT
  while (true) {
    cout << "Tick" << endl;
    tree.rootNode()->executeTick();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

int main(int argc, char *argv[]) {
  run_from_groot();
  return 0;
}
