#pragma once

#include <QWidget>
#include <QPalette>
#include <QwtPlotCanvas>


struct QAdvPlotCanvas_Settings {
    QPalette background_color { Qt::white  };
};

class QAdvPlotCanvas : public QwtPlotCanvas
{
    Q_OBJECT;
public:
    QAdvPlotCanvas(QwtPlot* parent = nullptr);
    QAdvPlotCanvas(const QAdvPlotCanvas_Settings& settings, QwtPlot* parent = nullptr);
    ~QAdvPlotCanvas() noexcept override;
};

