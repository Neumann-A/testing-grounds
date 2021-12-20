#include <qsize.h>
#include "qt-adv-plot.hpp"

#include "plot.hpp"

#include <QApplication>
#include <QMainWindow>
#include <QWidget>

class QtAdvPlot_App : public QApplication {
private:
    QMainWindow mainWindow;
    QAdvPlot *myplot;

protected:
    //bool event(QEvent *e) override;
public:
    QtAdvPlot_App(int argc, char *argv[]) : QApplication(argc,argv), mainWindow() {
        mainWindow.setObjectName(QString::fromUtf8("main_window"));
        mainWindow.resize(QSize(600,400));
        myplot = new QAdvPlot({}, &mainWindow);
        mainWindow.setCentralWidget(myplot);
        mainWindow.show();
    }
    ~QtAdvPlot_App() noexcept override {};
};

int main(int argc, char *argv[])
{
    QtAdvPlot_App app(argc,argv);

    return app.exec();
}
