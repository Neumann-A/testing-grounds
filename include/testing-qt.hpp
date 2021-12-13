#pragma once

#include <qdockwidget.h>
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QVector>
#include <DockManager.h>

class QMenuBar;
class QMenu;
class QAction;
class QStatusBar;


class Qt_TestApplication : public QApplication {
private:
    QMainWindow mainWindow;
    ads::CDockManager* m_DockManager;

    //Menu stuff
    QMenuBar *menubar = nullptr;
    QMenu    *menuViews = nullptr;
    QAction  *menuViewsLogs = nullptr;

    //Status Bar
    QStatusBar *statusbar;

    //Main Widgets
    void setUpMenu(QMenuBar* bar);
    void setUpActions(QMenu* menu);
    void setUpContent(QMainWindow* main);
    void setUpStatus(QMainWindow* main);
protected:
    //bool event(QEvent *e) override;
public:
    Qt_TestApplication(int argc, char *argv[]);
    ~Qt_TestApplication() noexcept override;
};
