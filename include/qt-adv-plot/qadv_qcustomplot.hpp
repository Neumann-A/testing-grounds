#pragma once

#include <qcustomplot.h>

class QWidget;

class QAdvCustomPlot : public QCustomPlot
{
public:
    QAdvCustomPlot(QWidget * parent = nullptr);
    ~QAdvCustomPlot() override;
};
