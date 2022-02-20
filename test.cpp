#include <qdebug.h>
#include <qmainwindow.h>
#include <QMetaObject>
#include <QMetaProperty>
#include <QApplication>
#include <QDebug>

#include "test_lib.hpp"

void printProperties(const QObject& obj) {
    auto meta_obj = obj.metaObject();
    qDebug() << "Classname: " << meta_obj->className();
    for(auto i = 1; i < meta_obj->propertyCount(); i++) {
        auto meta_property = meta_obj->property(i);
        auto type_name = meta_property.typeName();
        qDebug() << QStringLiteral("Type: %0").arg(QString::fromLatin1(meta_property.typeName()));
    }
}

class My_Test_App :public QApplication
{
private:
    QMainWindow mainWindow;
    MyTest testobj;
public:
    My_Test_App(int argc, char *argv[]) : QApplication(argc,argv), mainWindow(), testobj() {
        printProperties(testobj);
        mainWindow.show();
    }
};

#include <stdio.h>

template <typename T>
constexpr auto typenameHelper()
{
#ifdef _MSC_VER
    return __FUNCSIG__;
#else
    return __PRETTY_FUNCTION__;
#endif
}

int main(int argc, char *argv[])
{
    My_Test_App app(argc,argv);
    //printf("%s\n", typenameHelper<int>());
    return app.exec();
}

