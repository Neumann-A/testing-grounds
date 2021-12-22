#pragma once

#include <QWidget>
#include <QwtPlotLayout>

class QAdvPlotLayout : public QwtPlotLayout {
public:
    QAdvPlotLayout();
    ~QAdvPlotLayout() noexcept override;
protected:
    void activate( const QwtPlot* plot,
        const QRectF& plotRect, Options options ) override;
};
