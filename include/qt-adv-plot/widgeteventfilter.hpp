#pragma once

#include <qobject.h>

#include <Qt6/QtCore/qtmetamacros.h>
#include <QObject>



class WidgetEventFilter : public QObject {
private:
    Q_OBJECT;
public:
    WidgetEventFilter(QObject *object = nullptr);
    bool eventFilter(QObject *object, QEvent *event) override;
    Q_SIGNAL void widgetChanged(QObject *object);
};

