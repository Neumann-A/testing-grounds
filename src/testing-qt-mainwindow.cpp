#include "testing-qt-mainwindow.hpp"
//#include "ui_testing-qt-mainwindow.hpp"
TestMainWindow::TestMainWindow(QWidget *parent, Qt::WindowFlags flags) 
    : QMainWindow(parent, flags), m_DockManager(new ads::CDockManager(this)) {

}

TestMainWindow::~TestMainWindow() {
   // delete ui;
};

#include "moc_testing-qt-mainwindow.cpp"