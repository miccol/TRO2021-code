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
#include <absolute_barrier.h>
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

#include <chrono>

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::microseconds us;
typedef std::chrono::duration<float> fsec;

using namespace BT;

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


double run_absolute_barriers(AbsoluteBarrier barrier, double a1, double w_max1,
                             int num_of_children) {

    BT::ParallelNode parallel("root", num_of_children);
    std::vector<PseudoRandom*> children_nodes;
    std::vector<DecoratorProgressSync*> dec_nodes;

    for(int i = 0; i < num_of_children; i++)
    {

        PseudoRandom* action_1 = new PseudoRandom("Arm Movement", a1, w_max1);
        DecoratorProgressSync* dec1 = new DecoratorProgressSync("dec1", &barrier);
        dec1->addChild(action_1);
        children_nodes.push_back(action_1);
        dec_nodes.push_back(dec1);
        parallel.addChild(dec1);
    }

    vector<QLineSeries *> data = {};

    for(auto child : parallel.children())
    {
        ((PseudoRandom*)child)->set_progress(0.0);
    }

    int i = 0;
    double diff = 0.0;
    bool done = false;

    while (!done) {
        done = true;
        for(PseudoRandom* child : children_nodes)
        {
            done = done && child->progress() >= 1;
        }


        for(PseudoRandom* child : children_nodes)
        {
            for(PseudoRandom* other_child : children_nodes)
            {
                diff = diff + abs(child->progress() - other_child->progress())/2.0;
            }
        }


        parallel.executeTick();

        i++;
    }

    parallel.halt();

    for (auto p : children_nodes)
    {
        delete p;
    }
    children_nodes.clear();

    for (auto p : dec_nodes)
    {
        delete p;
    }
    dec_nodes.clear();



    return diff/i;
}

double run_async_absolute_barriers(AbsoluteBarrier barrier, double a1, double w_max1,
                             int num_of_children) {

    BT::ParallelNode parallel("root", num_of_children);
    std::vector<PseudoRandom*> children_nodes;
    std::vector<DecoratorProgressSync*> dec_nodes;

    for(int i = 0; i < num_of_children; i++)
    {

        PseudoRandom* action_1 = new PseudoRandom("Arm Movement", a1, w_max1);
        children_nodes.push_back(action_1);
        parallel.addChild(action_1);
    }

    vector<QLineSeries *> data = {};

    for(auto child : parallel.children())
    {
        ((PseudoRandom*)child)->set_progress(0.0);
    }

    int i = 0;
    double diff = 0.0;
    bool done = false;

    while (!done) {
        done = true;
        for(PseudoRandom* child : children_nodes)
        {
            done = done && child->progress() >= 1;
        }


        for(PseudoRandom* child : children_nodes)
        {
            for(PseudoRandom* other_child : children_nodes)
            {
                diff = diff + abs(child->progress() - other_child->progress())/2.0;
            }
        }


        parallel.executeTick();

        i++;
    }

    parallel.halt();

    for (auto p : children_nodes)
    {
        delete p;
    }
    children_nodes.clear();

    for (auto p : dec_nodes)
    {
        delete p;
    }
    dec_nodes.clear();



    return diff/i;
}

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    std::vector<double> num_of_children_values = {2, 4, 8, 16};

    QBoxPlotSeries *sync_series_005 = new QBoxPlotSeries();
    QBoxPlotSeries *sync_series_01 = new QBoxPlotSeries();
    QBoxPlotSeries *sync_series_1 = new QBoxPlotSeries();

    sync_series_005->setName("|B|= 20");
    sync_series_01->setName("|B|= 10");
    sync_series_1->setName("|B|= 0");

    //std::vector<double> average_results;

    float plot_pos = 0.5;
    for (double num_of_children : num_of_children_values) {

        cout << "Computing values for num_of_children = " << num_of_children << endl;



        std::vector<double> barrier_values;
        int i = 0;
//        while (i++ < 5) {
//            barrier_values.push_back(i / double(5));
//        }
        barrier_values.push_back(1.0);
//        barrier_values.push_back(1.0);

        std::cout << "5 barriers" << std::endl;

        for(auto b : barrier_values)
        {
            std::cout << "b - " << b << std::endl;
        }
        AbsoluteBarrier barrier5(barrier_values);

        barrier_values.clear();
        i = 0;
        while (i++ < 10) {
            barrier_values.push_back(i / double(10));
        }
        barrier_values.push_back(1.0);

        std::cout << "10 barriers" << std::endl;

        for(auto b : barrier_values)
        {
            std::cout << "b - " << b << std::endl;
        }
        AbsoluteBarrier barrier10(barrier_values);

        barrier_values.clear();
        i = 0;
        while (i++ < 20) {
            barrier_values.push_back(i / double(20));
        }
        barrier_values.push_back(1.0);
        std::cout << "20 barriers"  << std::endl;

        for(auto b : barrier_values)
        {
            std::cout << "b - " << b << std::endl;
        }
        AbsoluteBarrier barrier20(barrier_values);


        double a1 = 0.03;
        double w_max1 = a1 / 2.0;

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

        double result_0 = 0;
        double result_10 = 0;
        double result_20 = 0;

        QList<qreal> sortedList_005, sortedList_01, sortedList_1;
        QBoxSet *box_sync_005 = new QBoxSet(to_string(num_of_children).c_str());
        QBoxSet *box_sync_01 = new QBoxSet(to_string(num_of_children).c_str());
        QBoxSet *box_sync_1 = new QBoxSet(to_string(num_of_children).c_str());

        while (episode++ < 10) {
            cout << "Episode = " << episode << endl;

            result_0 = run_async_absolute_barriers(barrier5, a1, w_max1, num_of_children);
            result_10 = run_absolute_barriers(barrier10, a1, w_max1, num_of_children);
            result_20 = run_absolute_barriers(barrier20, a1, w_max1, num_of_children);

            sortedList_1.push_back(result_0);
            sortedList_01.push_back(result_10);
            sortedList_005.push_back(result_20);

            average = average + result_0;
        }

        //average_results.push_back(average);
        std::sort(sortedList_005.begin(), sortedList_005.end());
        int count = sortedList_005.count();
        qreal median = findMedian(sortedList_005, 0, count);
        qreal Q1 = findMedian(sortedList_005, 0, count / 2);
        qreal Q3 = findMedian(sortedList_005, count / 2 + (count % 2), count);
        qreal IQR = Q3 - Q1;

        box_sync_005->setValue(QBoxSet::Median, median);
        box_sync_005->setValue(QBoxSet::LowerQuartile, Q1);
        box_sync_005->setValue(QBoxSet::UpperQuartile, Q3);
        box_sync_005->setValue(QBoxSet::LowerExtreme, sortedList_005.first());
        box_sync_005->setValue(QBoxSet::UpperExtreme, Q3 + 1.5 * IQR);

        plot_pos = plot_pos + 0.5;

        std::sort(sortedList_01.begin(), sortedList_01.end());

        int count_5 = sortedList_01.count();
        qreal median_5 = findMedian(sortedList_01, 0, count_5);
        qreal Q1_5 = findMedian(sortedList_01, 0, count_5 / 2);
        qreal Q3_5 = findMedian(sortedList_01, count_5 / 2 + (count_5 % 2), count_5);
        qreal IQR_5 = Q3_5 - Q1_5;
        box_sync_01->setValue(QBoxSet::Median, median_5);
        box_sync_01->setValue(QBoxSet::LowerQuartile, Q1_5);
        box_sync_01->setValue(QBoxSet::UpperQuartile, Q3_5);
        box_sync_01->setValue(QBoxSet::LowerExtreme, sortedList_01.first());
        box_sync_01->setValue(QBoxSet::UpperExtreme, Q3_5 + 1.5 * IQR_5);

        plot_pos = plot_pos + 0.5;

        std::sort(sortedList_1.begin(), sortedList_1.end());

        int count_20 = sortedList_1.count();
        qreal median_20 = findMedian(sortedList_1, 0, count_20);
        qreal Q1_20 = findMedian(sortedList_1, 0, count_20 / 2);
        qreal Q3_20 =
                findMedian(sortedList_1, count_20 / 2 + (count_20 % 2), count_20);
        qreal IQR_20 = Q3_20 - Q1_20;
        box_sync_1->setValue(QBoxSet::Median, median_20);
        box_sync_1->setValue(QBoxSet::LowerQuartile, Q1_20);
        box_sync_1->setValue(QBoxSet::UpperQuartile, Q3_20);
        box_sync_1->setValue(QBoxSet::LowerExtreme, sortedList_1.first());
        box_sync_1->setValue(QBoxSet::UpperExtreme, Q3_20 + 1.5 * IQR_20);
        ;
        plot_pos = plot_pos + 1.0;

        sync_series_005->append(box_sync_005);
        sync_series_01->append(box_sync_01);
        sync_series_1->append(box_sync_1);
    }
    QChart *chart = new QChart();
    chart->addSeries(sync_series_1);
    chart->addSeries(sync_series_01);
    chart->addSeries(sync_series_005);

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
