#include "qadv_canvas.hpp"

#include <QwtPlotCanvas>

QAdvPlotCanvas::QAdvPlotCanvas(QwtPlot* parent) : QwtPlotCanvas(parent) {
    this->setFrameStyle( (int)QFrame::NoFrame | (int)QFrame::Plain );
    this->setContentsMargins(0, 0, 0, 0);
}
QAdvPlotCanvas::QAdvPlotCanvas(const QAdvPlotCanvas_Settings& settings, QwtPlot* parent) : QAdvPlotCanvas(parent) {
    this->setPalette(settings.background_color);
}


QAdvPlotCanvas::~QAdvPlotCanvas() noexcept {

};

#include "moc_qadv_canvas.cpp"
