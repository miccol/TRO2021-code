/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file example5.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */

#include <behaviortree_cpp_v3/behavior_tree.h>
#include <chrono> // std::chrono::seconds
#include <iostream>
#include <thread> // std::this_thread::sleep_for

#include <decorator_progress_sync.h>
#include <relative_barrier.h>
#include <sync_smooth_action.h>

#include <QXYSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <data_plotter.h>

#include <QFloat16>

#include <fstream>
#include <math.h>
#include <thread>
QT_CHARTS_USE_NAMESPACE

double sigmoid(double x) {
  double exp_value;
  double return_value;

  /*** Exponential calculation ***/
  exp_value = exp((double)-x);

  /*** Final sigmoid value ***/

  return_value = (1 + tanh(3 * x / 10.0 - 3.0)) / 2.0;

  return return_value;
}

using namespace BT;
using namespace std;
QT_CHARTS_USE_NAMESPACE

class SigmoidProfile : public SyncSmoothAction {
public:
  SigmoidProfile(std::string name)
      : SyncSmoothAction(name, 0, 0), started(false) {}
  NodeStatus tick() override {

    if (!started) {
      started = true;
      std::thread t1(&SigmoidProfile::executeProfile, this);
      t1.detach();
    }
    return NodeStatus::SUCCESS;
  }

  void stop() {
    started = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  void executeProfile() {
    double i = 0;
    while (started) {

      set_progress(min(sigmoid(i), 1.0));
      std::cout << i << "New progress profile value of " << progress()
                << std::endl;

      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      i = i + 0.1;
    }
  }

private:
  atomic<bool> started;
};

vector<QLineSeries *> run_profile() {
  BT::ParallelNode parallel("root", 2);

  SyncSmoothAction action_1("Action", 0, 0.01);
  SigmoidProfile action_2("Profile");

  RelativeBarrier barrier(0.001);

  DecoratorProgressSync dec1("dec1", &barrier);
  DecoratorProgressSync dec2("dec2", &barrier);

  dec1.addChild(&action_1);
  dec2.addChild(&action_2);

  parallel.addChild(&dec1);
  parallel.addChild(&dec2);

  NodeStatus status = NodeStatus::RUNNING;

  QLineSeries *series_1 = new QLineSeries();
  series_1->setName(action_1.name().c_str());

  QLineSeries *series_2 = new QLineSeries();
  series_2->setName(action_2.name().c_str());

  vector<QLineSeries *> data = {series_1, series_2};

  int i = 0;
  series_1->append(i, action_1.progress());
  series_2->append(i, action_2.progress());
  // series_3->append(i, action_3.progress());
  while (action_1.progress() < 1 || action_2.progress() < 0.99) {
    i++;
    status = parallel.executeTick();

    series_1->append(i, action_1.progress());
    series_2->append(i, action_2.progress());

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  action_2.stop();
  parallel.halt();
  return data;
}

vector<QLineSeries *> run_unsynchronized() {
  BT::ParallelNode parallel("root", 2);

  SyncSmoothAction action_1("Action", 0, 0.01);
  SigmoidProfile action_2("Profile");

  RelativeBarrier barrier(0.001);

  DecoratorProgressSync dec1("dec1", &barrier);
  DecoratorProgressSync dec2("dec2", &barrier);

  parallel.addChild(&action_1);
  parallel.addChild(&action_2);

  NodeStatus status = NodeStatus::RUNNING;

  QLineSeries *series_1 = new QLineSeries();
  series_1->setName(action_1.name().c_str());

  QLineSeries *series_2 = new QLineSeries();
  series_2->setName(action_2.name().c_str());

  vector<QLineSeries *> data = {series_1};

  int i = 0;
  series_1->append(i, action_1.progress());
  while (action_1.progress() < 1) {
    i++;

    status = parallel.executeTick();
    series_1->append(i, action_1.progress());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  action_2.stop();
  parallel.halt();
  return data;
}

int main(int argc, char *argv[]) {

  QApplication a(argc, argv);

  vector<QLineSeries *> data_sync = run_profile();
  vector<QLineSeries *> data_unsync = run_unsynchronized();

  DataPlotter *plotter_sync = new DataPlotter();
  QChartView *chartView_sync =
      plotter_sync->plot("Progress Profiles. Synchronized Case", data_sync);

  DataPlotter *plotter_unsync = new DataPlotter();
  QChartView *chartView_unsync = plotter_unsync->plot(
      "Progress profiles. Unsynchronized Case", data_unsync);

  QMainWindow window_sync;
  window_sync.setCentralWidget(chartView_sync);
  window_sync.resize(820, 600);
  window_sync.show();

  QMainWindow window_unsync;
  window_unsync.setCentralWidget(chartView_unsync);
  window_unsync.resize(820, 600);
  window_unsync.show();

  return a.exec();
}
