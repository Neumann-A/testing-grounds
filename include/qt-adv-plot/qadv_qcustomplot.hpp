#pragma once

#include <qcustomplot.h>
#include <QtTreePropertyBrowser>
#include <QtVariantEditorFactory>
#include <QtVariantProperty>
#include <QtVariantPropertyManager>




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
    QtVariantPropertyManager *variantManager;
    QtVariantEditorFactory *variantEditor;
    QtTreePropertyBrowser *browser;
};
