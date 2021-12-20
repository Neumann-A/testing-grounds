#include "layout.hpp"
#include <QwtPlotLayout>
#include <QwtScaleWidget>
#include <QwtScaleDraw>
#include <QwtScaleDiv>
#include <QRectF>
#include <QDebug>

QAdvPlotLayout::QAdvPlotLayout() : QwtPlotLayout() {};
QAdvPlotLayout::~QAdvPlotLayout() noexcept {};

void QAdvPlotLayout::activate( const QwtPlot* plot,
    const QRectF& plotRect, Options options ) {
    QwtPlotLayout::activate(plot, plotRect, options);

    const auto axisID = QwtPlot::xTop;
    const auto axisWidget = plot->axisWidget(axisID);
    if(plot->isAxisVisible(axisID) && axisWidget->alignment() == QwtScaleDraw::BottomScale)
    {
        QRectF scaleRect = this->scaleRect(QwtPlot::xTop);
        const auto ticklength = axisWidget->scaleDraw()->tickLength(QwtScaleDiv::TickType::MajorTick);
        scaleRect.setY(scaleRect.y()+ticklength+1);
        scaleRect.setHeight(scaleRect.height()+ticklength+1);
        this->setScaleRect(axisID, scaleRect);
        //axisWidget->scaleDraw()->
    }
    // for ( int axisPos = 0; axisPos < QwtAxis::AxisPositions; axisPos++ )
    // {
    //     // set the rects for the axes

    //     const int pos = 0;
    //     {
    //         const QwtAxisId axisId( axisPos );

    //         const QRectF& canvasRect = this->canvasRect();

    //         QRectF scaleRect = this->scaleRect(axisId);
    //         scaleRect = canvasRect;

    //         switch ( axisPos )
    //         {
    //             case QwtAxis::YLeft:
    //             {
    //                 const auto dim = canvasRect.width();
    //                 scaleRect.setX( canvasRect.left() - pos - dim );
    //                 scaleRect.setWidth( dim );
    //                 break;
    //             }
    //             case QwtAxis::YRight:
    //             {
    //                 const auto dim = canvasRect.width();
    //                 scaleRect.setX( canvasRect.right() + pos );
    //                 scaleRect.setWidth( dim );
    //                 break;
    //             }
    //             case QwtAxis::XBottom:
    //             {
    //                 const auto dim = canvasRect.height();
    //                 scaleRect.setY( canvasRect.bottom() + pos );
    //                 scaleRect.setHeight( dim );
    //                 break;
    //             }
    //             case QwtAxis::XTop:
    //             {
    //                 const auto dim = canvasRect.height();
    //                 scaleRect.setY( canvasRect.top() - pos - dim );
    //                 scaleRect.setHeight( dim );
    //                 break;
    //             }
    //         }
    //         scaleRect = scaleRect.normalized();
    //         this->setScaleRect(axisId,scaleRect);
    //     }
    // }
}
