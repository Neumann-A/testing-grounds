
#include "widgeteventfilter.hpp"

#include <QEvent>

WidgetEventFilter::WidgetEventFilter(QObject *parent) : QObject(parent) {};
bool WidgetEventFilter::eventFilter(QObject *object, QEvent *event)
{
    if( event->type() == QEvent::Resize || 
        event->type() == QEvent::DynamicPropertyChange ||
        event->type() == QEvent::WindowStateChange) {
        Q_EMIT widgetChanged(object);
    }
    return object->event(event);
}


#include "moc_widgeteventfilter.cpp"


