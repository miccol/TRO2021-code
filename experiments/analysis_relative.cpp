/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file analysis_relative.cpp
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
#include <QFloat16>
#include <fstream>
#include <random>

using namespace BT;
using namespace std;
QT_CHARTS_USE_NAMESPACE

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

double run_relative_barriers(RelativeBarrier barrier, double a1, double w_max1,
                             double a2, double w_max2) {

  BT::ParallelNode parallel("root", 2);

  PseudoRandom action_1("Arm Movement", a1, w_max1);
  PseudoRandom action_2("Base Movement", a2, w_max2);

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

  action_1.set_progress(0.0);
  action_2.set_progress(0.0);

  vector<QLineSeries *> data = {series_1, series_2};
  int i = 0;
  double diff = 0.0;
  while (action_1.progress() < 1 || action_2.progress() < 1) {

    diff = diff + abs(action_1.progress() - action_2.progress()) / 2.0;
    status = parallel.executeTick();
    i++;
  }
  series_1->append(i, action_1.progress());
  series_2->append(i, action_2.progress());

  parallel.halt();
  return diff;
}

double run_async(double a1, double w_max1, double a2, double w_max2) {

  BT::ParallelNode parallel("root", 2);

  PseudoRandom action_1("Arm Movement", a1, w_max1);
  PseudoRandom action_2("Base Movement", a2, w_max2);

  parallel.addChild(&action_1);
  parallel.addChild(&action_2);

  NodeStatus status = NodeStatus::RUNNING;

  QLineSeries *series_1 = new QLineSeries();
  series_1->setName(action_1.name().c_str());

  QLineSeries *series_2 = new QLineSeries();
  series_2->setName(action_2.name().c_str());

  action_1.set_progress(0.0);
  action_2.set_progress(0.0);

  vector<QLineSeries *> data = {series_1, series_2};
  int i = 0;
  double diff = 0.0;
  while (action_1.progress() < 1 || action_2.progress() < 1) {

    diff = diff + abs(action_1.progress() - action_2.progress()) / 2.0;
    status = parallel.executeTick();
    i++;
  }
  series_1->append(i, action_1.progress());
  series_2->append(i, action_2.progress());

  parallel.halt();
  return diff;
}

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);
  std::vector<double> delta_values = {1, 0.1, 0.05, 0.02};

  QBoxPlotSeries *sync_series = new QBoxPlotSeries();
  QBoxPlotSeries *sync_series_5 = new QBoxPlotSeries();
  QBoxPlotSeries *sync_series_20 = new QBoxPlotSeries();

  sync_series->setName("Disturbance 10%");
  sync_series_5->setName("Disturbance 5%");
  sync_series_20->setName("Disturbance 2%");

  std::vector<double> average_results;

  float plot_pos = 0.5;
  for (double x : delta_values) {

    cout << "Computing values for Delata = " << x << endl;

    RelativeBarrier barrier(x);

    double a1 = 0.05;
    double w_max1 = 0.05;

    double a2 = 0.01;
    double w_max2 = 0.05;

    std::random_device
        rd1; // Will be used to obtain a seed for the random number engine
    std::mt19937 gen1(
        rd1()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis1(0.03, 0.3);

    std::random_device
        rd2; // Will be used to obtain a seed for the random number engine
    std::mt19937 gen2(
        rd2()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis2(0.02, 0.2);

    int episode = 0;

    double average = 0;

    double result = 0;
    double result_5 = 0;
    double result_20 = 0;

    QList<qreal> sortedList, sortedList_5, sortedList_20;
    QBoxSet *box_sync = new QBoxSet(to_string(x).c_str());
    QBoxSet *box_sync_5 = new QBoxSet(to_string(x).c_str());
    QBoxSet *box_sync_20 = new QBoxSet(to_string(x).c_str());

    while (episode++ < 1000) {
      a1 = 0.03;
      a2 = 0.02;

      w_max1 = a1 / 2.0;
      w_max2 = a2 / 2.0;

      result = run_relative_barriers(barrier, a1, w_max1, a2, w_max2);

      w_max1 = a1 / 5.0;
      w_max2 = a2 / 5.0;

      result_5 = run_relative_barriers(barrier, a1, w_max1, a2, w_max2);
      w_max1 = 0.0006;
      w_max2 = 0.0008;

      w_max1 = a1 / 10.0;
      w_max2 = a2 / 10.0;

      result_20 = run_relative_barriers(barrier, a1, w_max1, a2, w_max2);

      sortedList_20.push_back(result_20);
      sortedList_5.push_back(result_5);
      sortedList.push_back(result);

      average = average + result;
    }

    average = average / x;

    average_results.push_back(average);
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
    ;
    plot_pos = plot_pos + 1.0;

    sync_series->append(box_sync);
    sync_series_5->append(box_sync_5);
    sync_series_20->append(box_sync_20);
  }
  QChart *chart = new QChart();
  chart->addSeries(sync_series_20);
  chart->addSeries(sync_series_5);
  chart->addSeries(sync_series);

  QValueAxis *axisX = new QValueAxis;

  axisX->setTickCount(2); // from qt 5.12 use setTickIncrement(1);
  axisX->setLabelFormat("%d");
  axisX->setTitleText("w_max");
  axisX->setTitleText("Number of Barriers");

  QValueAxis *axisY = new QValueAxis;
  // axisY->setRange(0, 1);
  axisY->setTickCount(20); // from qt 5.12 use setTickIncrement(1);
  axisY->setLabelFormat("%0.1f");
  axisY->setTitleText("Progress Distance");
  chart->addAxis(axisY, Qt::AlignLeft);

  chart->createDefaultAxes();
  chart->axisX()->setTitleText("Value of Delta");
  chart->axisY()->setTitleText("Progress Distance");

  QChartView *chartView = new QChartView(chart);
  QMainWindow window;
  window.setCentralWidget(chartView);
  window.resize(820, 600);
  window.show();
  return app.exec();
}
