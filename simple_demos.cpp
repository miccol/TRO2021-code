/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file simple_demos.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */


#include <iostream>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <behaviortree_cpp_v3/bt_factory.h>

#include <smooth_action.h>
#include <absolute_barrier.h>
#include <relative_barrier.h>
#include <decorator_progress_sync.h>
#include <sync_smooth_action.h>
#include <async_smooth_action.h>

#include <decorator_resource_sync.h>
#include <resource_handler.h>

#include <data_plotter.h>

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

using namespace BT;
using namespace std;
QT_CHARTS_USE_NAMESPACE


vector<QLineSeries*> run_resource()
{

    // 1 Create the root and leaf nodes
    BT::ParallelNode parallel("root",2);

    SyncSmoothAction action_1("Action 1", 1000, 0.1);
    SyncSmoothAction action_2("Action 2", 1000, 0.1);

    // 2 set the resources allocated by the action
    action_1.allocateResources({"foo", "baz"});
    action_2.allocateResources({"bar"});

    // 3 Create the resource decorator. In this example we have 2 decorators.
    // Decorators share the same resource handler. We could have different sincronizations by defining different resource handlers.
    ResourceHandler resource_handler;
    DecoratorResourceSync dec1("dec1", &resource_handler);
    DecoratorResourceSync dec2("dec2", &resource_handler);

    // 4 Construct the BT
    dec1.addChild(&action_1);
    dec2.addChild(&action_2);

    parallel.addChild(&dec1);
    parallel.addChild(&dec2);

    NodeStatus status = NodeStatus::RUNNING;

    // 5 Initialize QT LineSeries for visualization purposes
    QLineSeries* series_1 = new QLineSeries();
    series_1->setName(action_1.name().c_str());

    QLineSeries* series_2 = new QLineSeries();
    series_2->setName(action_2.name().c_str());

    vector<QLineSeries*> data = {series_1, series_2};

    // 6 Execute the BT
    int i=0;
    while (action_1.progress() < 1 || action_2.progress() < 1)
    {

        std::cout << "Tick # " << i << std::endl;

        // 7 Save data to visualize

        series_1->append(i, action_1.progress());
        series_2->append(i, action_2.progress());

        status = parallel.executeTick();
        if (i==3)
        {
            std::cout <<  "Action 2 acquires resource foo " << std::endl;
            action_2.allocateResource("foo");

        }
        if (i==6)
        {
            std::cout <<  "Action 1 releases foo " << std::endl;
            action_1.releaseResource("foo");

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i++;
    }

    parallel.halt();

    return data;
}


vector<QLineSeries*> run_absolute_barriers()
{

    // 1 Create the root and leaf nodes

    BT::ParallelNode parallel("root",2);

    SyncSmoothAction action_1("Action 1", 100, 0.1);
    SyncSmoothAction action_2("Action 2", 100, 0.05);

    // 2 Set the value for the absolute barriers
    AbsoluteBarrier barrier({0.0, 0.1, 0.2, 0.3 ,0.4,
                             0.5, 0.6, 0.7, 0.8, 0.9, 1.0});

    // 3 Construct the BT
    DecoratorProgressSync dec1("dec1", &barrier);
    DecoratorProgressSync dec2("dec2", &barrier);

    dec1.addChild(&action_1);
    dec2.addChild(&action_2);

    parallel.addChild(&dec1);
    parallel.addChild(&dec2);

    NodeStatus status = NodeStatus::RUNNING;

    // 4 Initialize QT LineSeries for visualization purposes

    QLineSeries* series_1 = new QLineSeries();
    series_1->setName(action_1.name().c_str());

    QLineSeries* series_2 = new QLineSeries();
    series_2->setName(action_2.name().c_str());

    vector<QLineSeries*> data = {series_1, series_2};

    // 5 Execute the BT
    int i=0;
    while (action_1.progress() < 1 || action_2.progress() < 1)
    {

        std::cout << "Tick # " << i << std::endl;

        // 6 Save data to visualize
        series_1->append(i, action_1.progress());
        series_2->append(i, action_2.progress());

        status = parallel.executeTick();

        i++;
    }


    parallel.halt();
    return data;
}

vector<QLineSeries*> run_relative_barriers()
{

    // 1 Create root and leaf nodes
    BT::ParallelNode parallel("root",3);

    SyncSmoothAction action_1("Action 1", 100, 0.01);
    SyncSmoothAction action_2("Action 2", 100, 0.05);
    SyncSmoothAction action_3("Action 3", 100, 0.015);

    // 2 Define the value of Delta
    RelativeBarrier barrier(0.01);

    // 3 Create the Decorator nodes. We may implement different sychronization instances by defining separate RelativeBarrier object
    DecoratorProgressSync dec1("dec1", &barrier);
    DecoratorProgressSync dec2("dec2", &barrier);
    DecoratorProgressSync dec3("dec3", &barrier);

    // 4 Construct the BT
    dec1.addChild(&action_1);
    dec2.addChild(&action_2);
    dec3.addChild(&action_3);

    parallel.addChild(&dec1);
    parallel.addChild(&dec2);
    parallel.addChild(&dec3);

    NodeStatus status = NodeStatus::RUNNING;

    // 5 Initialize QT LineSeries for visualization purposes

    QLineSeries* series_1 = new QLineSeries();
    series_1->setName(action_1.name().c_str());

    QLineSeries* series_2 = new QLineSeries();
    series_2->setName(action_2.name().c_str());

    QLineSeries* series_3 = new QLineSeries();
    series_3->setName(action_3.name().c_str());


    vector<QLineSeries*> data = {series_1, series_2, series_3};

    // 6 Execute the BT

    int i=0;
    series_1->append(i, action_1.progress());
    series_2->append(i, action_2.progress());
    series_3->append(i, action_3.progress());
    while (action_1.progress() < 1 || action_2.progress() < 1 || action_3.progress() < 1)
    {
        i++;
        std::cout << "Tick # " << i << std::endl;

        status = parallel.executeTick();

        // 6 Save data to visualize
        series_1->append(i, action_1.progress());
        series_2->append(i, action_2.progress());
        series_3->append(i, action_3.progress());
    }

    parallel.halt();
    return data;
}


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    vector<QLineSeries*> data ={};
    int x = 0;

    while (x < 1 || x > 3)
    {
        cout << "Select the Demo and press Enter " << endl;
        cout << "1: Absolute Progress Synchronization " << endl;
        cout << "2: Relative Progress Synchronization " << endl;
        cout << "3: Resource Synchronization " << endl;
        cin >> x; // Get user input from the keyboard
    }

    switch (x) {
    case 1:
        cout << "Running Absolute Progress Synchronization " << endl;
        data = run_absolute_barriers();
        break;
    case 2:
        cout << "Running Relative Progress Synchronization " << endl;
        data = run_relative_barriers();
        break;
    case 3:
        cout << "Running Resource Synchronization " << endl;
        data = run_resource();
        break;
    default:
        break;
    }
    // plotting data
    DataPlotter* plotter = new DataPlotter();
    QChartView* chartView = plotter->plot("Progress Profiles", data);
    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(820, 600);
    window.show();

    return a.exec();
}



