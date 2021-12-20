#pragma once

#include "scale.hpp"
#include "canvas.hpp"

#include <QwtPlot>

#include <optional>

struct QAdvPlot_Settings {
    QString title {"QAdvPlot Plot Widget"};
    QString x_axis_label {"x-axis"};
    QString y_axis_label {"y-axis"};
    QAdvPlotCanvas_Settings canvas_settings{};
};


struct QAdvPlot : public QwtPlot {
    Q_OBJECT;
public:
    QAdvPlot(const QAdvPlot_Settings& settings, QWidget * parent = nullptr);
    QAdvPlot(QWidget * parent = nullptr);
    ~QAdvPlot() noexcept override;
};
