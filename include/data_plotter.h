#ifndef DATA_PLOTTER_H
#define DATA_PLOTTER_H
#include <QtCharts>
#include <QLineSeries>


class DataPlotter
{
public:
    DataPlotter();
    QtCharts::QChartView *plot(std::string title, const std::vector<QtCharts::QLineSeries *> series_vect);
};

#endif // DATA_PLOTTER_H
