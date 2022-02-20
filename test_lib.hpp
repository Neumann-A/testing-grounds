#pragma once
#include <qtmetamacros.h>
#include <QObject>
#include <QCursor>
#include <QPen>

class MyTest : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QCursor* pcursor READ pcursor)
    Q_PROPERTY(QCursor cursor READ cursor)
    QCursor cursor() { return *m_pcursor; }
    QCursor* pcursor() { return m_pcursor; }

private:
    QCursor* m_pcursor;
};

