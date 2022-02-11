
#include "widgeteventfilter.hpp"

WidgetEventFilter::WidgetEventFilter(QObject *parent) : QObject(parent) {};
bool WidgetEventFilter::eventFilter(QObject *object, QEvent *event)
{
    Q_EMIT widgetChanged(object);
    return object->event(event);
}


#include "moc_widgeteventfilter.cpp"


