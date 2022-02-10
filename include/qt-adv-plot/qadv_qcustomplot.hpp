#pragma once

#include <qcustomplot.h>





class QWidget;
class QContextMenuEvent;

class QAdvCustomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    QAdvCustomPlot(QWidget * parent = nullptr);
    ~QAdvCustomPlot() override;
    void contextMenuEvent(QContextMenuEvent *event) override;
private:

};
