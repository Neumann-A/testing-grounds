#pragma once

#include <QWidget>
#include <QwtScaleDraw>
#include <QwtScaleWidget>

class QAdvScaleDraw : public QwtScaleDraw {
public:
    QAdvScaleDraw();
    ~QAdvScaleDraw() noexcept override;
protected:
    // void drawTick( QPainter*, double value, double len ) const override;
    // void drawLabel( QPainter* painter, double value ) const override;
    // void drawBackbone(QPainter * painter) const override;
};

class QAdvScaleWidget : public QwtScaleWidget {
public:
    QAdvScaleWidget(QWidget * parent = nullptr);
    ~QAdvScaleWidget() noexcept override;
protected:
    //QwtScaleDraw::Alignment revertScaleAlignment(const QwtScaleDraw::Alignment& input) const ;
};

