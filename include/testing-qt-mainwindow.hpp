#pragma once

#include <QWidget>
#include <QMainWindow>
#include <DockManager.h>

// namespace Ui {
// class TestMainWindow;
// }

class TestMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit TestMainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~TestMainWindow() override;

private:
    // Ui::TestMainWindow *ui;
    
    // The main container for docking
    ads::CDockManager* m_DockManager;
};
