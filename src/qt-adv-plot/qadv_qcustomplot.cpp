
#include "qadv_qcustomplot.hpp"
#include <qcustomplot.h>

QAdvCustomPlot::QAdvCustomPlot(QWidget * parent) : QCustomPlot(parent) {
        auto plotwidget = this;
        plotwidget->xAxis2->setVisible(true);
        plotwidget->yAxis2->setVisible(true);
        plotwidget->xAxis2->setTickLabels(false);
        plotwidget->yAxis2->setTickLabels(false);

        {
            auto pen = plotwidget->xAxis->basePen();
            pen.setWidth(2);
            plotwidget->xAxis->setBasePen(pen);
            plotwidget->xAxis->setTickPen(pen);
            plotwidget->xAxis->setSubTickPen(pen);
        }
        {
            auto pen = plotwidget->xAxis2->basePen();
            pen.setWidth(2);
            plotwidget->xAxis2->setBasePen(pen);
            plotwidget->xAxis2->setTickPen(pen);
            plotwidget->xAxis2->setSubTickPen(pen);
        }
        {
            auto pen = plotwidget->yAxis->basePen();
            pen.setWidth(2);
            plotwidget->yAxis->setBasePen(pen);
            plotwidget->yAxis->setTickPen(pen);
            plotwidget->yAxis->setSubTickPen(pen);
        }
        {
            auto pen = plotwidget->yAxis2->basePen();
            pen.setWidth(2);
            plotwidget->yAxis2->setBasePen(pen);
            plotwidget->yAxis2->setTickPen(pen);
            plotwidget->yAxis2->setSubTickPen(pen);
        }
};
QAdvCustomPlot::~QAdvCustomPlot() {

};
