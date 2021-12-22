#include "qadv_plot.hpp"
#include "qadv_canvas.hpp"
#include "qadv_scale.hpp"
#include "qadv_layout.hpp"
#include <QWidget>
#include <QwtPlotLayout>
#include <QwtScaleWidget>
#include <QwtScaleDraw>
#include <QwtPlotScaleItem>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>
#include <QwtPlotMagnifier>

QAdvPlot::QAdvPlot(QWidget * parent) : QwtPlot(parent) {
    this->setPlotLayout(new QAdvPlotLayout());
    //enable all axis
    this->enableAxis(QwtPlot::yLeft);
    this->enableAxis(QwtPlot::yRight);
    this->enableAxis(QwtPlot::xBottom);
    this->enableAxis(QwtPlot::xTop);

    //Closes the gap between axis and canvas
    this->axisWidget(QwtPlot::yLeft)->setMargin(0);
    this->axisWidget(QwtPlot::yLeft)->setSpacing(0);
    this->axisWidget(QwtPlot::yRight)->setMargin(0);
    this->axisWidget(QwtPlot::xBottom)->setMargin(0);
    this->axisWidget(QwtPlot::xTop)->setMargin(0);
    this->axisWidget(QwtPlot::xTop)->setSpacing(0);


    // plot.canvas()->setStyleSheet("QwtPlotCanvas {border: none; margin: 1; background-color: white;}")
    this->axisWidget(QwtPlot::xTop)->setAlignment(QwtScaleDraw::BottomScale);
    this->axisWidget(QwtPlot::xTop)->setScaleDraw(new QAdvScaleDraw());
    this->axisWidget(QwtPlot::xTop)->scaleDraw()->enableComponent(QwtAbstractScaleDraw::Labels, false);
    //this->axisWidget(QwtPlot::xTop)->scaleDraw()->setPenWidthF(2);

    this->axisWidget(QwtPlot::xTop)->setStyleSheet("border: 1px solid red");
    //this->axisWidget(QwtPlot::xTop)->setMinBorderDist(0, 0);
    //this->axisWidget(QwtPlot::xTop)->setColorBarEnabled(false);

    this->axisWidget(QwtPlot::xTop)->scaleDraw()->setSpacing(0);
    this->axisWidget(QwtPlot::xTop)->scaleDraw()->setLabelAlignment(Qt::AlignHCenter | Qt::AlignTop);
    //this->axisWidget(QwtPlot::xTop)->scaleDraw()->setAlignment(QwtScaleDraw::TopScale);
    //this->axisWidget(QwtPlot::xTop)->setLayoutFlag(LayoutFlag, bool on);

    //this->axisWidget(QwtPlot::xTop)->scaleDraw()->setTickLength(QwtScaleDiv::MajorTick,0);
    //this->axisWidget(QwtPlot::xTop)->scaleDraw()->setTickLength(QwtScaleDiv::MinorTick,0);
    //this->axisWidget(QwtPlot::xTop)->scaleDraw()->setTickLength(QwtScaleDiv::MediumTick,0);

    //QwtPlotScaleItem *xTopScaleItem = new QwtPlotScaleItem(QwtScaleDraw::BottomScale, this->axisWidget(QwtPlot::xTop)->scaleDraw()->pos().y());
    //xTopScaleItem->scaleDraw()->enableComponent(QwtAbstractScaleDraw::Labels, false);
    //xTopScaleItem->attach(this);
    //xTopScaleItem->setXAxis(QwtPlot::xTop);
    //xTopScaleItem->setBorderDistance(0);
    //xTopScaleItem->scaleDraw()->setPenWidthF(2);

}

QAdvPlot::~QAdvPlot() noexcept {

}

QAdvPlot::QAdvPlot(const QAdvPlot_Settings& settings, QWidget * parent) : QAdvPlot(parent) {
    this->setCanvas(new QAdvPlotCanvas(settings.canvas_settings,this));
    this->plotLayout()->setCanvasMargin(0);
    this->updateCanvasMargins();
    this->canvas()->setVisible(false);
    //( void ) new QwtPlotZoomer ( this->canvas() );
    //( void ) new QwtPlotPanner( this->canvas() );
    //( void ) new QwtPlotMagnifier( this->canvas() );
    this->canvas()->stackUnder(this->axisWidget(QwtPlot::xTop));
    //this->axisWidget(QwtPlot::xTop)->stackUnder(this->canvas());
}

#include "moc_qadv_plot.cpp"
