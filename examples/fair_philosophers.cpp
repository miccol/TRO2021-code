/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
/**
 * @file main.cpp
 * @authors: Michele Colledanchise <michele.colledanchise@iit.it>
 */

#ifdef ZMQ_FOUND
#include "behaviortree_cpp_v3/loggers/bt_zmq_publisher.h"
#endif

#include <iostream>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/actions/always_failure_node.h>
#include <behaviortree_cpp_v3/actions/always_success_node.h>



#include <behaviortree_cpp_v3/behavior_tree.h>
#include <behaviortree_cpp_v3/bt_factory.h>
#include <smooth_action.h>
#include <absolute_barrier.h>
#include <relative_barrier.h>
#include <decorator_progress_sync.h>
#include <sync_smooth_action.h>
#include <async_smooth_action.h>

#include <decorator_resource_sync.h>
#include <resource_handler.h>



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
#include <random>

#include <QFloat16>
#include <fstream>
using namespace BT;
using namespace std;
QT_CHARTS_USE_NAMESPACE


class Philosopher :  public SyncSmoothAction{
private:
    bool m_is_eating;
    std::vector<string> m_chopstick_names;
public:
    Philosopher(std::string name, std::vector<string> chopstick_names) : SyncSmoothAction(name, 0,0), m_is_eating(false), m_chopstick_names(chopstick_names)
    {
        allocateResources(m_chopstick_names);

    }
    NodeStatus tick() override
    {
        if (progress() >=1.0)
        {
            releaseResources(m_chopstick_names);

            return NodeStatus::SUCCESS;
        }
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> dis(0.0, 1.0);

        double x =  dis(gen);
        double progress_increment = 0.1;

        set_progress(max(min(progress() + progress_increment, 1.0), 0.0));

        return NodeStatus::SUCCESS;
    }

};



vector<QLineSeries*> run_resource()
{

    BT::ParallelNode parallel("root",3);

    Philosopher action_1("Philosopher 1", {"A", "B"});
    Philosopher action_2("Philosopher 2", {"B", "C"});
    Philosopher action_3("Philosopher 3", {"A", "C"});

    ResourceHandler resource_handler( [](double x) { return x + 1.0; });
    DecoratorResourceSync dec1("dec1", &resource_handler);
    DecoratorResourceSync dec2("dec2", &resource_handler);
    DecoratorResourceSync dec3("dec3", &resource_handler);

    dec1.addChild(&action_1);
    dec2.addChild(&action_2);
    dec3.addChild(&action_3);

    parallel.addChild(&dec1);
    parallel.addChild(&dec2);
    parallel.addChild(&dec3);

    NodeStatus status = NodeStatus::RUNNING;

    QLineSeries* series_1 = new QLineSeries();
    series_1->setName(action_1.name().c_str());

    QLineSeries* series_2 = new QLineSeries();
    series_2->setName(action_2.name().c_str());

    QLineSeries* series_3 = new QLineSeries();
    series_3->setName(action_3.name().c_str());


    vector<QLineSeries*> data = {series_1, series_2, series_3};

    int i=0;
    while (action_1.progress() < 1 || action_2.progress() < 1 || action_3.progress() < 1)
    {
        series_1->append(i, action_1.progress());
        series_2->append(i, action_2.progress());
        series_3->append(i, action_3.progress());

        status = parallel.executeTick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        i++;
    }

    parallel.halt();

    return data;
}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    vector<QLineSeries*> data = run_resource();
    DataPlotter* plotter = new DataPlotter();
    QChartView* chartView = plotter->plot("Progress profiles", data);
    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(820, 600);
    window.show();

    return a.exec();
}
