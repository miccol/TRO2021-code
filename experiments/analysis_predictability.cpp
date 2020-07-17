/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file analysis_predictability.cpp
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

#include <QBoxPlotSeries>
#include <fstream>
#include <random>

using namespace BT;
using namespace std;
QT_CHARTS_USE_NAMESPACE

double sigmoid(double x) {
  double exp_value;
  double return_value;

  /*** Exponential calculation ***/
  exp_value = exp((double)-x);

  /*** Final sigmoid value ***/

  return_value = (1 + tanh(3 * x - 3.0)) / 2.0;

  return return_value;
}

using namespace BT;
using namespace std;
QT_CHARTS_USE_NAMESPACE

class SigmoidProfile : public SyncSmoothAction {
private:
  atomic<bool> started;

public:
  SigmoidProfile(std::string name)
      : SyncSmoothAction(name, 0, 0), started(false) {}
  ~SigmoidProfile() {}
  NodeStatus tick() override {
    set_progress(min(progress() + 0.001, 1.0));
    return NodeStatus::SUCCESS;
  }
  void stop() { started = false; }
};

class PseudoRandom : public SyncSmoothAction {
public:
  PseudoRandom(std::string name, double a, double w_max)
      : SyncSmoothAction(name, 0, 0), w_max_(w_max), a_(a) {}
  NodeStatus tick() override {
    std::random_device
        rd; // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(-w_max_, w_max_);

    double disturbance = dis(gen);
    double progress_increment = a_ + disturbance;

    set_progress(max(min(progress() + progress_increment, 1.0), 0.0));

    return NodeStatus::SUCCESS;
  }

private:
  double w_max_;
  double a_;
};

qreal findMedian(QList<qreal> sortedList, int begin, int end) {
  int count = end - begin;
  if (count % 2) {
    return sortedList.at(count / 2.0 + begin);
  } else {
    qreal right = sortedList.at(count / 2.0 + begin);
    qreal left = sortedList.at(count / 2.0 - 1 + begin);
    return (right + left) / 2.0;
  }
}

double run_profile(RelativeBarrier barrier, double a1, double w_max1, double a2,
                   double w_max2) {

  BT::ParallelNode parallel("root", 2);

  PseudoRandom action_1("Action", a1, w_max1);
  SigmoidProfile action_2("Profile");

  DecoratorProgressSync dec1("dec1", &barrier);
  DecoratorProgressSync dec2("dec2", &barrier);

  dec1.addChild(&action_1);
  dec2.addChild(&action_2);

  parallel.addChild(&dec1);
  parallel.addChild(&dec2);

  NodeStatus status = NodeStatus::RUNNING;

  action_1.set_progress(0.0);
  action_2.set_progress(0.0);

  int i = 0;
  int actual_time = 0.0;
  double closest_to_desired_progress = 0.0;

  int expected_time = 200;
  double desired_progress = 0.2;
  double current_progress = 0.0;

  while (action_1.progress() < 0.25 || action_2.progress() < 0.25) {
    current_progress = action_1.progress();

    if (abs(current_progress - desired_progress) <
        abs(closest_to_desired_progress - desired_progress)) {
      closest_to_desired_progress = current_progress;
      actual_time = i;
    }

    status = parallel.executeTick();
    i++;
  }
  action_2.stop();
  return abs(actual_time - expected_time);
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  float plot_pos = 0.5;
  std::vector<double> delta_values = {1, 0.1, 0.05, 0.005};

  QBoxPlotSeries *sync_series = new QBoxPlotSeries();
  QBoxPlotSeries *sync_series_5 = new QBoxPlotSeries();
  QBoxPlotSeries *sync_series_20 = new QBoxPlotSeries();

  sync_series->setName("Disturbance 10%");
  sync_series_5->setName("Disturbance 5%");
  sync_series_20->setName("Disturbance 2%");

  for (double x : delta_values) {
    cout << "Computing values for Delta = " << x << endl;

    RelativeBarrier barrier(x);

    double a1 = 0.05;
    double w_max1 = 0.05;

    double a2 = 0.01;
    double w_max2 = 0.05;

    int episode = 0;

    double average = 0;

    double result = 0;
    double result_5 = 0;
    double result_20 = 0;

    QList<qreal> sortedList, sortedList_5, sortedList_20;
    QBoxSet *box_sync = new QBoxSet(to_string(x).c_str());
    QBoxSet *box_sync_5 = new QBoxSet(to_string(x).c_str());
    QBoxSet *box_sync_20 = new QBoxSet(to_string(x).c_str());

    while (episode++ < 10000) {
      a1 = 0.03;
      a2 = 0.02;

      w_max1 = a1 / 2.0;
      w_max2 = a2 / 2.0;

      result = run_profile(barrier, a1, w_max1, a2, w_max2);

      w_max1 = a1 / 5.0;
      w_max2 = a2 / 5.0;

      result_5 = run_profile(barrier, a1, w_max1, a2, w_max2);

      w_max1 = a1 / 10.0;
      w_max2 = a2 / 10.0;

      result_20 = run_profile(barrier, a1, w_max1, a2, w_max2);

      sortedList_20.push_back(result_20);
      sortedList_5.push_back(result_5);
      sortedList.push_back(result);

      average = average + result;
    }

    std::sort(sortedList.begin(), sortedList.end());
    int count = sortedList.count();

    qreal median = findMedian(sortedList, 0, count);
    qreal Q1 = findMedian(sortedList, 0, count / 2);
    qreal Q3 = findMedian(sortedList, count / 2 + (count % 2), count);
    qreal IQR = Q3 - Q1;

    box_sync->setValue(QBoxSet::Median, median);
    box_sync->setValue(QBoxSet::LowerQuartile, Q1);
    box_sync->setValue(QBoxSet::UpperQuartile, Q3);
    box_sync->setValue(QBoxSet::LowerExtreme, sortedList.first());
    box_sync->setValue(QBoxSet::UpperExtreme, Q3 + 1.5 * IQR);

    plot_pos = plot_pos + 0.5;

    std::sort(sortedList_5.begin(), sortedList_5.end());

    int count_5 = sortedList_5.count();
    qreal median_5 = findMedian(sortedList_5, 0, count_5);
    qreal Q1_5 = findMedian(sortedList_5, 0, count_5 / 2);
    qreal Q3_5 = findMedian(sortedList_5, count_5 / 2 + (count_5 % 2), count_5);
    qreal IQR_5 = Q3_5 - Q1_5;
    box_sync_5->setValue(QBoxSet::Median, median_5);
    box_sync_5->setValue(QBoxSet::LowerQuartile, Q1_5);
    box_sync_5->setValue(QBoxSet::UpperQuartile, Q3_5);
    box_sync_5->setValue(QBoxSet::LowerExtreme, sortedList_5.first());
    box_sync_5->setValue(QBoxSet::UpperExtreme, Q3_5 + 1.5 * IQR_5);

    plot_pos = plot_pos + 0.5;

    std::sort(sortedList_20.begin(), sortedList_20.end());

    int count_20 = sortedList_20.count();
    qreal median_20 = findMedian(sortedList_20, 0, count_20);
    qreal Q1_20 = findMedian(sortedList_20, 0, count_20 / 2);
    qreal Q3_20 =
        findMedian(sortedList_20, count_20 / 2 + (count_20 % 2), count_20);
    qreal IQR_20 = Q3_20 - Q1_20;
    box_sync_20->setValue(QBoxSet::Median, median_20);
    box_sync_20->setValue(QBoxSet::LowerQuartile, Q1_20);
    box_sync_20->setValue(QBoxSet::UpperQuartile, Q3_20);
    box_sync_20->setValue(QBoxSet::LowerExtreme, sortedList_20.first());
    box_sync_20->setValue(QBoxSet::UpperExtreme, Q3_20 + 1.5 * IQR_20);

    plot_pos = plot_pos + 1.0;
    sync_series->append(box_sync);
    sync_series_5->append(box_sync_5);
    sync_series_20->append(box_sync_20);
  }

  QChart *chart = new QChart();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  chart->addSeries(sync_series);
  chart->addSeries(sync_series_5);
  chart->addSeries(sync_series_20);

  QValueAxis *axisX = new QValueAxis;

  axisX->setTickCount(2); // from qt 5.12 use setTickIncrement(1);
  axisX->setLabelFormat("%d");
  axisX->setTitleText("w_max");
  axisX->setTitleText("Number of Barriers");

  QValueAxis *axisY = new QValueAxis;
  axisY->setTickCount(20); // from qt 5.12 use setTickIncrement(1);
  axisY->setLabelFormat("%0.1f");
  axisY->setTitleText("Progress Distance");
  chart->addAxis(axisY, Qt::AlignLeft);

  chart->createDefaultAxes();
  chart->axisX()->setTitleText("Value of Delta");
  chart->axisY()->setTitleText("Predictability Distancea");

  QChartView *chartView = new QChartView(chart);
  QMainWindow window;
  window.setCentralWidget(chartView);
  window.resize(820, 600);
  window.show();
  app.exec();

  return 0;
}
