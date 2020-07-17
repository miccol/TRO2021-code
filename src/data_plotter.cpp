#include <data_plotter.h>
#include <QtCharts>
#include <QLineSeries>
#include <QLineSeries>
#include <QValueAxis>
using namespace QtCharts;

DataPlotter::DataPlotter()
{

}

QChartView* DataPlotter::plot(std::string title , const std::vector<QLineSeries*> series_vect)
{

    //![3]
    QChart *chart = new QChart();
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->show();
    chart->setTitle(title.c_str());

    QValueAxis *axisX = new QValueAxis;

    axisX->setTickCount(std::min(series_vect.at(0)->count(), 20)); // from qt 5.12 use setTickIncrement(1);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("k");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 1);
    axisY->setTickCount(20); // from qt 5.12 use setTickIncrement(1);
    axisY->setLabelFormat("%0.2f");
    axisY->setTitleText("Progress");
    chart->addAxis(axisY, Qt::AlignLeft);

    for(QLineSeries* series : series_vect)
    {
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        series->setPointsVisible();
    }
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;

}

