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

    Q_SIGNAL void propertyChanged();
protected:
    bool event(QEvent *event) override;



};
