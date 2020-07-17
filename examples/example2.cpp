/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file example2.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */

#include <iostream>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <thread> // std::this_thread::sleep_for
#include <chrono> // std::chrono::seconds

#include <relative_barrier.h>
#include <decorator_progress_sync.h>
#include <sync_smooth_action.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QValueAxis>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCharts/QValueAxis>
#include <QXYSeries>
#include <data_plotter.h>

#include <QFloat16>
#include <fstream>

using namespace BT;
using namespace std;
QT_CHARTS_USE_NAMESPACE

vector<QLineSeries*> run_relative_barriers()
{
    BT::ParallelNode parallel("root", 2);

    SyncSmoothAction action_1("Action 1", 0, 0.01);
    SyncSmoothAction action_2("Action 2", 0, 0.005);

    RelativeBarrier barrier(0.1);

    DecoratorProgressSync dec1("dec1", &barrier);
    DecoratorProgressSync dec2("dec2", &barrier);

    dec1.addChild(&action_1);
    dec2.addChild(&action_2);

    parallel.addChild(&dec1);
    parallel.addChild(&dec2);

    NodeStatus status = NodeStatus::RUNNING;

    QLineSeries* series_1 = new QLineSeries();
    series_1->setName(action_1.name().c_str());

    QLineSeries* series_2 = new QLineSeries();
    series_2->setName(action_2.name().c_str());

    vector<QLineSeries*> data = { series_1, series_2 };

    int i = 0;
    series_1->append(i, action_1.progress());
    series_2->append(i, action_2.progress());
    while (action_1.progress() < 1 || action_2.progress() < 1) {
        i++;
        status = parallel.executeTick();

        series_1->append(i, action_1.progress());
        series_2->append(i, action_2.progress());

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    parallel.halt();
    return data;
}

vector<QLineSeries*> run_unsynchronized()
{
    BT::ParallelNode parallel("root", 2);

    SyncSmoothAction action_1("Action 1", 0, 0.01);
    SyncSmoothAction action_2("Action 2", 0, 0.005);

    parallel.addChild(&action_1);
    parallel.addChild(&action_2);

    NodeStatus status = NodeStatus::RUNNING;

    QLineSeries* series_1 = new QLineSeries();
    series_1->setName(action_1.name().c_str());

    QLineSeries* series_2 = new QLineSeries();
    series_2->setName(action_2.name().c_str());

    vector<QLineSeries*> data = { series_1, series_2 };

    int i = 0;
    series_1->append(i, action_1.progress());
    series_2->append(i, action_2.progress());
    while (action_1.progress() < 1 || action_2.progress() < 1) {
        i++;
        status = parallel.executeTick();

        series_1->append(i, action_1.progress());
        series_2->append(i, action_2.progress());

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    parallel.halt();
    return data;
}

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);

    vector<QLineSeries*> data_sync = run_relative_barriers();
    vector<QLineSeries*> data_unsync = run_unsynchronized();

    DataPlotter* plotter_sync = new DataPlotter();
    QChartView* chartView_sync = plotter_sync->plot("Progress Profiles. Synchronized Case", data_sync);

    DataPlotter* plotter_unsync = new DataPlotter();
    QChartView* chartView_unsync = plotter_unsync->plot("Progress profiles. Unsynchronized Case", data_unsync);

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
