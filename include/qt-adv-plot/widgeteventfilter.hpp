#pragma once

#include <qobject.h>
#include <qobjectdefs.h>

#include <Qt6/QtCore/qtmetamacros.h>
#include <QObject>
#include <QMetaProperty>
#include <QtVariantProperty>

class PropertyNotifyBinder : public QObject{
public:
    QObject* sender;
    QMetaProperty property;
    QtVariantProperty* varproperty;

    PropertyNotifyBinder(QObject* send, QMetaProperty prop, QtVariantProperty* varprop) :
        sender(send), property(prop), varproperty(varprop) { 
            if(prop.hasNotifySignal()) {
                const auto metamethod = prop.notifySignal();
                QString sig = QString("SIGNAL(%1)").arg(metamethod.methodSignature());
                qDebug() << "Metamethod, sig:" << metamethod.methodSignature();
                qDebug() << "Metamethod, sig, string:" << sig;
                //auto metasend = send->metaObject();
                auto metarecieve = this->metaObject();
                auto irec = metarecieve->indexOfSlot(QMetaObject::normalizedSignature("forward()"));
                qDebug() << "irec:" << irec;
                bool ok = QObject::connect(sender, metamethod, this, metarecieve->method(irec));
                qDebug() << "Connect OK? " << ok;
            }
         }
private:
    Q_OBJECT
    Q_SLOT void forward() {
        auto rp = property.read(sender);
        varproperty->setValue(rp);
        qDebug() << "Forwarded:" << rp; 
    }
};

class WidgetEventFilter : public QObject {
private:
    Q_OBJECT;
public:
    WidgetEventFilter(QObject *object = nullptr);
    bool eventFilter(QObject *object, QEvent *event) override;
    Q_SIGNAL void widgetChanged(QObject *object);
};

