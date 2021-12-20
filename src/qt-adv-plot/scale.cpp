#include "scale.hpp"

#include <QwtScaleMap>
#include <QwtScaleDiv>
#include <QPainter>
#include <QwtPainter>

inline qreal penWidth( const QPainter* painter, const QwtScaleDraw* scaleDraw )
{
    qreal width = scaleDraw->penWidthF();
#if 1
    if ( width <= 0.0 )
        width = 1.0;
#endif

    if ( painter->pen().isCosmetic() )
    {
        const QTransform& transform = painter->transform();

        switch ( scaleDraw->alignment() )
        {
            case QwtScaleDraw::LeftScale:
            case QwtScaleDraw::RightScale:
            {
                width /= transform.m11();
                break;
            }
            case QwtScaleDraw::TopScale:
            case QwtScaleDraw::BottomScale:
            {
                width /= transform.m22();
                break;
            }
        }
    }

    return width;
}

QAdvScaleDraw::QAdvScaleDraw() : QwtScaleDraw() {

}

QAdvScaleDraw::~QAdvScaleDraw() noexcept {

}

// void QAdvScaleDraw::drawTick( QPainter* painter, double value, double tickLength ) const {
//     if ( tickLength <= 0 )
//         return;

//     const double tickPos = scaleMap().transform( value );

//     const auto scaleDraw = this;

//     const QPointF pos = scaleDraw->pos();

//     qreal pw = 0.0;

//     if ( scaleDraw->hasComponent( QwtScaleDraw::Backbone ) )
//         pw = penWidth( painter, scaleDraw );

//     const qreal length = tickLength + pw;

//     /*
//         Those correction offsets have been found by try and error.
//         They need to be understood and replaced by a calculation,
//         that makes sense. TODO ...
//         */
//     const qreal off1 = 1.0;
//     const qreal off2 = ( scaleDraw->penWidthF() <= 0.0 ) ? 0.5 : 0.0;

//     switch ( scaleDraw->alignment() )
//     {
//         case QwtScaleDraw::RightScale:
//         {
//             const qreal x = pos.x() + off1 - off2;
//             QwtPainter::drawLine( painter, x, tickPos, x - length, tickPos );

//             break;
//         }
//         case QwtScaleDraw::LeftScale:
//         {
//             const qreal x = pos.x() - off1 + off2;
//             QwtPainter::drawLine( painter, x, tickPos, x + length, tickPos );
//             break;
//         }
//         case QwtScaleDraw::BottomScale:
//         {
//             const qreal y = pos.y() + off1 - 2 * off2;
//             QwtPainter::drawLine( painter, tickPos, y, tickPos, y - length );

//             break;
//         }
//         case QwtScaleDraw::TopScale:
//         {
//             const qreal y = pos.y() - off1 + off2;
//             QwtPainter::drawLine( painter, tickPos, y, tickPos, y + length );

//             break;
//         }
//     }
// }
static QwtScaleDraw::Alignment revertScaleAlignment(const QwtScaleDraw::Alignment& input) {
    switch(input) {
        case QwtScaleDraw::BottomScale:
            return QwtScaleDraw::TopScale;
        case QwtScaleDraw::TopScale:
            return QwtScaleDraw::BottomScale;
        case QwtScaleDraw::LeftScale:
            return QwtScaleDraw::RightScale;
        case QwtScaleDraw::RightScale:
            return QwtScaleDraw::LeftScale;
    }
}

// void QAdvScaleDraw::drawLabel( QPainter* painter, double value ) const
// {
//     // 
//     //
//     // const auto ticklength = this->tickLength(QwtScaleDiv::MajorTick);
//     // const_cast<QAdvScaleDraw*>(this)->setTickLength(QwtScaleDiv::MajorTick, 0);
//     // QwtScaleDraw::drawLabel(painter,value);
    
//     // 
//     // const_cast<QAdvScaleDraw*>(this)->setTickLength(QwtScaleDiv::MajorTick, ticklength);
//     QwtText lbl = tickLabel( painter->font(), value );
//     if ( lbl.isEmpty() )
//         return;

//     const QwtScaleDraw::Alignment current = this->alignment();
//     const_cast<QAdvScaleDraw*>(this)->setAlignment(revertScaleAlignment(current));
//     QPointF pos = labelPosition( value );
//     const_cast<QAdvScaleDraw*>(this)->setAlignment(current);

//     QSizeF labelSize = lbl.textSize( painter->font() );

//     const QTransform transform = labelTransformation( pos, labelSize );

//     painter->save();
//     painter->setWorldTransform( transform, true );

//     lbl.draw ( painter, QRect( QPoint( 0, 0 ), labelSize.toSize() ) );

//     painter->restore();
// }

// void QAdvScaleDraw::drawBackbone (QPainter * painter) const {
//     auto scaleDraw = this;
//     const qreal pw2 = 0.5 * penWidth( painter, scaleDraw );

//     const QPointF pos = scaleDraw->pos();
//     const qreal length = scaleDraw->length();

//     switch ( scaleDraw->alignment() )
//     {
//         case QwtScaleDraw::LeftScale:
//         {
//             const qreal x = pos.x() + 0.5 - pw2;
//             QwtPainter::drawLine( painter, x/2, pos.y(), x/2, pos.y() + length );

//             break;
//         }
//         case QwtScaleDraw::RightScale:
//         {
//             const qreal x = pos.x() - 0.5 + pw2;
//             QwtPainter::drawLine( painter, x/2, pos.y(), x/2, pos.y() + length );

//             break;
//         }
//         case QwtScaleDraw::TopScale:
//         {
//             const qreal y = pos.y() + 0.5 - pw2;
//             QwtPainter::drawLine( painter, pos.x(), y/2, pos.x() + length, y/2 );

//             break;
//         }
//         case QwtScaleDraw::BottomScale:
//         {
//             const qreal y = pos.y() - 0.5 + pw2;
//             QwtPainter::drawLine( painter, pos.x(), y/2, pos.x() + length, y/2 );

//             break;
//         }
//     }
// }

QAdvScaleWidget::QAdvScaleWidget(QWidget * parent) : QwtScaleWidget(parent) {
    // setAttribute(Qt::WA_NoSystemBackground);
    // setAttribute(Qt::WA_TranslucentBackground);
    // setAttribute(Qt::WA_PaintOnScreen);
};
QAdvScaleWidget::~QAdvScaleWidget() noexcept  {};
