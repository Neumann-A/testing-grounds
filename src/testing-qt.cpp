
#include "testing-qt.hpp"
#include "testing-qt-mainwindow.hpp"
#include <ads_globals.h>
#include <qapplication.h>
#include <qgridlayout.h>
#include <qopenglwidget.h>
#include <qsettings.h>

#include <QVariant>
#include <QAction>
#include <QIcon>
#include <QDockWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QListView>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QFont>

#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtPlotCanvas>
#include <QwtPlotPanner>
#include <QwtPlotMagnifier>
#include <QwtPlotPicker>
#include <QwtPlotZoomer>
#include <QwtLegend>
#include <QwtKnob>
#include <QwtAnalogClock>
#include <QwtSlider>
#include <QwtScaleWidget>
#include <QwtScaleDraw>
#include <QwtScaleDiv>

#include <DockManager.h>
#include <qwt_axis.h>
#include <QOpenGLWidget>

#include <imgui.h>
#include <implot.h>
#include <QtImGui.h>

struct QDockInit {
    QFlags<ads::CDockWidget::DockWidgetFeature> features {ads::CDockWidget::DefaultDockWidgetFeatures};
    ads::DockWidgetArea             area{ads::CenterDockWidgetArea};
    ads::DockWidgetAreas            allowed;
    bool floating {false};
    //bool visible {true};
    QString title {QString::fromUtf8("Not set")};
};

static  ads::CDockWidget * setUpNewDockWidget(QMainWindow& main, ads::CDockManager& manager, QMenu& menu, QDockInit init) {
    static std::size_t counter = 0;
    ads::CDockWidget * pdock = new  ads::CDockWidget(init.title,&main);
    ads::CDockWidget & dock = *pdock; 
    dock.setFeatures(init.features);
    if((init.features & ads::CDockWidget::DockWidgetFloatable) ? init.floating : false) {
        dock.setFloating( );
    }
    dock.setVisible(true);
#ifdef _DEBUG
    dock.setWindowTitle(init.title + QString("_0x") +  QString::number((std::size_t)(pdock),16));
#endif
    dock.setObjectName(QString{"dock_"}.append(QString::number(counter++)).append("_").append(init.title.toLower()));
    if(init.features & ads::CDockWidget::DockWidgetClosable) {
        menu.addAction(dock.toggleViewAction());
    }

    manager.addDockWidgetTab(init.area,&dock);
    return pdock;
}

Qt_TestApplication::~Qt_TestApplication() noexcept {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,"MyTestComp", "NyTesting");
    settings.setValue("geometry", mainWindow.saveGeometry());
    settings.setValue("windowstate", mainWindow.saveState());
    settings.setValue("dockstate", m_DockManager->saveState());
    auto Childlist = mainWindow.children();
    qDebug() << Childlist;
};
Qt_TestApplication::Qt_TestApplication(int argc, char *argv[]) : QApplication(argc,argv), mainWindow() {
    mainWindow.setObjectName(QString::fromUtf8("main_window"));
    mainWindow.layout()->setObjectName(QString::fromUtf8("main_layout"));

    //m_DockManager->setStyleSheet("");

    statusbar = new QStatusBar(&mainWindow);
    statusbar->setObjectName(QString::fromUtf8("main_status_bar"));
    mainWindow.setStatusBar(statusbar);

    menubar = mainWindow.menuBar(); //new QMenuBar(&mainWindow);
    menubar->setObjectName(QString::fromUtf8("main_menu_bar"));
    auto viewsmenu = menubar->addMenu(tr("&Views"));
    // auto logviewaction = viewsmenu->addAction("&Logs");
    // logviewaction->setCheckable(true);
    // logviewaction->setChecked(true);

    //mainWindow.setCentralWidget(nullptr);

    //mainWindow.setCentralWidget(new QWidget(&mainWindow));
    //const auto centralWidget = mainWindow.centralWidget();
    //centralWidget->setObjectName(QString::fromUtf8("central_widget"));
    //centralWidget->hide();
    //centralWidget->setMinimumSize(QSize(200,200));

    //centralWidget->setLayout(new QGridLayout(centralWidget));
    //auto centralLayout = static_cast<QGridLayout * const>(centralWidget->layout());
    //centralLayout->setObjectName(QString::fromUtf8("central_layout"));

    mainWindow.setDockOptions(QMainWindow::DockOption::AllowNestedDocks 
                            | QMainWindow::DockOption::AllowTabbedDocks
                            | QMainWindow::DockOption::GroupedDragging);

    auto logDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="Logs"});
    auto logDockWidget = new QWidget(logDock);
    logDockWidget->setLayout(new QGridLayout(logDockWidget));
    logDockWidget->layout()->setObjectName(QString::fromUtf8("dock_log_widget_layout"));
    logDock->setWidget(logDockWidget);

    auto testdock_0 = setUpNewDockWidget(mainWindow,*m_DockManager, *viewsmenu,{.area=ads::RightDockWidgetArea,.title="Test0"});
    auto testwidget_0 = new QFrame(testdock_0);
    testwidget_0->setFrameShape( QFrame::Box); 
    testwidget_0->setLineWidth(10);
    testwidget_0->setLayout(new QGridLayout(testwidget_0));
    testwidget_0->layout()->setObjectName(QString::fromUtf8("dock_test_widget_0_layout"));
    testdock_0->setWidget(testwidget_0);
    auto plotwidget_0 = new QwtPlot(testwidget_0);
    //testwidget_0->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
    static_cast<QGridLayout*>(testwidget_0->layout())->addWidget(plotwidget_0,0,0,1,1);
    plotwidget_0->setTitle("My Plot");
    plotwidget_0->setAxisTitle(QwtAxis::XBottom,"x in a.u.");
    plotwidget_0->setAxisTitle(QwtAxis::YLeft,"y in a.u.");
    plotwidget_0->axisWidget(QwtAxis::YLeft)->scaleDraw()->setPenWidthF(2);
    plotwidget_0->axisWidget(QwtAxis::YLeft)->scaleDraw()->setTickLength(QwtScaleDiv::TickType::MajorTick, 20.0);
    auto label = plotwidget_0->axisWidget(QwtAxis::YLeft)->scaleDraw();
    //plotwidget_0->axisWidget(QwtAxis::YLeft)->scaleDraw()->enableComponent(ScaleComponent::)

    plotwidget_0->insertLegend( new QwtLegend(), QwtPlot::RightLegend );
    // canvas
    QwtPlotCanvas* canvas = new QwtPlotCanvas(plotwidget_0);
    canvas->setLineWidth( 2 );
    canvas->setFrameStyle( (int)QFrame::Box | (int)QFrame::Plain );
    plotwidget_0->setCanvas(canvas);
    canvas->setBorderRadius( 15 );
    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::WindowText, QColor( 133, 190, 232 ) );
    canvas->setPalette( canvasPalette );
    canvas->setBorderRadius(0.0);
    canvas->setFocusIndicator(QwtPlotAbstractCanvas::ItemFocusIndicator);

    ( void ) new QwtPlotZoomer ( canvas );
    //
    ( void ) new QwtPlotPicker ( canvas );
    // panning with the left mouse button
    ( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
    ( void ) new QwtPlotMagnifier( canvas );

    //plotwidget_0->setStyleSheet("border:10px solid rgb(0, 255, 0); ");
    //auto testwidget_0_1 = new QFrame(testdock_0);
    //testwidget_0_1->setFrameShape( QFrame::Box); 
    //testwidget_0_1->setLineWidth(10);
    //static_cast<QGridLayout*>(testwidget_0->layout())->addWidget(testwidget_0_1,1,0,1,1);
    
    //static_cast<QGridLayout*>(testwidget_0->layout())->setStyleSheet("border:10px solid rgb(0, 255, 0); ");

    auto tooldock = setUpNewDockWidget(mainWindow,*m_DockManager, *viewsmenu,{.title="Tools"});
    auto toolWidget = new QWidget(tooldock);
    toolWidget->setLayout(new QGridLayout(toolWidget));
    toolWidget->layout()->setObjectName(QString::fromUtf8("dock_tool_widget_layout"));
    static_cast<QGridLayout*>(toolWidget->layout())->addWidget(new QwtSlider(toolWidget),0,0,1,1);
    tooldock->setWidget(toolWidget);

    auto testdock_1 = setUpNewDockWidget(mainWindow,*m_DockManager, *viewsmenu,{.area=ads::BottomDockWidgetArea,.title="Test1"});
    auto testwidget_1 = new QWidget(testdock_1);
    testwidget_1->setLayout(new QGridLayout(testwidget_1));
    testwidget_1->layout()->setObjectName(QString::fromUtf8("dock_test_widget_1_layout"));
    static_cast<QGridLayout*>(testwidget_1->layout())->addWidget(new QwtAnalogClock(testwidget_1),0,0,1,1);
    testdock_1->setWidget(testwidget_1);
    

    auto testdock_2 = setUpNewDockWidget(mainWindow,*m_DockManager, *viewsmenu,{.area=ads::RightDockWidgetArea,.title="Test2"});
    auto testwidget_2 = new QWidget(testdock_2);
    testwidget_2->setLayout(new QGridLayout(testwidget_2));
    testwidget_2->layout()->setObjectName(QString::fromUtf8("dock_test_widget_2_layout"));
    static_cast<QGridLayout*>(testwidget_2->layout())->addWidget(new QwtKnob(testwidget_2),0,0,1,1);
    testdock_2->setWidget(testwidget_2);

    //mainWindow.tabifyDockWidget(logDock, tooldock);
    // logDock = new QDockWidget(&mainWindow);
    // logDock->setObjectName(QString::fromUtf8("DockLogView"));
    // logDock->setFloating(false);
    // logDock->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetVerticalTitleBar);
    // logDockWidget = new QWidget(logDock);
    // logDockWidget->setObjectName(QString::fromUtf8("dockWidgetContents_3"));

    // logDock->setWidget(logDockWidget);
    // logDock->setWindowTitle(QString::fromUtf8("Logs & Status"));
    // mainWindow.addDockWidget(Qt::BottomDockWidgetArea, logDock);
    // logDock->raise();

    
    //Setup Signals.
    //QObject::connect(logDock,&QDockWidget::visibilityChanged,[logviewaction](bool visible){ logviewaction->setChecked(visible);});
    //QObject::connect(logDock,&QDockWidget::visibilityChanged,logviewaction,&QAction::setChecked);
    //QObject::connect(logviewaction,&QAction::toggled,[this](bool checked){ logDock->setVisible(checked);});
    //QObject::connect(logviewaction,&QAction::toggled, logDock, &QDockWidget::setVisible);
    //QObject::connect(logviewaction,&QAction::checkableChanged,[this](bool checked){ logDock->setVisible(checked);});

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,"MyTestComp", "NyTesting");
    if(settings.contains("geometry")) {
        mainWindow.restoreGeometry(settings.value("geometry").toByteArray());
    }
    if(settings.contains("windowstate")) {
        mainWindow.restoreState(settings.value("windowstate").toByteArray());
    }
    if(settings.contains("dockstate")) {
        m_DockManager->restoreState(settings.value("dockstate").toByteArray());
    }
    mainWindow.show();

    auto Childlist = mainWindow.children();
    qDebug() << Childlist;
}



int main(int argc, char *argv[])
{
    Qt_TestApplication app(argc,argv);

    return app.exec();
}
