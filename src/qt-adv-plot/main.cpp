#include <qnamespace.h>
#include <qobject.h>
#include <qopenglextrafunctions.h>
#include <qopenglwidget.h>
#include <qsize.h>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>

#include <DockManager.h>
#include <qtvariantproperty.h>

#include "qt-adv-plot.hpp"

#include "qadv_plot.hpp"
#include "qadv_imgui.hpp"
#include "qadv_qcustomplot.hpp"

#include "widgeteventfilter.hpp"

#include <QtTreePropertyBrowser>
#include <QtVariantEditorFactory>
#include <QtVariantProperty>
#include <QtVariantPropertyManager>

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

class QtAdvPlot_App : public QApplication {
private:
    QMainWindow mainWindow;
    ads::CDockManager* m_DockManager;

    QAdvCustomPlot * customplot;

    QtVariantPropertyManager *variantManager;
    QtVariantEditorFactory *variantEditor;
    QtTreePropertyBrowser *browser;
    WidgetEventFilter *filter;

    QMap<QString, QtVariantProperty*> propMap;
protected:
    //bool event(QEvent *e) override;
public:
    QtAdvPlot_App(int argc, char *argv[]) : QApplication(argc,argv), mainWindow() {
        mainWindow.setObjectName(QString::fromUtf8("main_window"));
        mainWindow.resize(QSize(600,400));

        auto menubar = mainWindow.menuBar(); //new QMenuBar(&mainWindow);
        menubar->setObjectName(QString::fromUtf8("main_menu_bar"));
        auto viewsmenu = menubar->addMenu(tr("&Views"));

        ads::CDockManager::setConfigFlags(ads::CDockManager::DefaultOpaqueConfig);
        ads::CDockManager::setConfigFlag(ads::CDockManager::RetainTabSizeWhenCloseButtonHidden, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHideDisabledButtons, true);
        m_DockManager = new ads::CDockManager(&mainWindow);

        {   
            auto qwtPlotDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="qwt-plot"});
            qwtPlotDock->setWidget(new QAdvPlot({}, &mainWindow));
        }
        {
            customplot = new QAdvCustomPlot(&mainWindow);
            auto qcustomplotPlotDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="qcustomplot-plot"});
            qcustomplotPlotDock->setWidget(customplot);
        }
        {
            variantManager = new QtVariantPropertyManager(&mainWindow);
            connect(variantManager, &QtVariantPropertyManager::valueChanged, this,
                        [=,this](QtProperty * prop, const QVariant & variant){
                            customplot->setProperty(prop->propertyName().toUtf8().data(), variant);
                        });

            variantEditor = new QtVariantEditorFactory(this);
            browser = new QtTreePropertyBrowser(&mainWindow);
            browser->setFactoryForManager(variantManager, variantEditor);

            filter = new WidgetEventFilter(customplot);
            customplot->installEventFilter(filter);
            connect(filter, &WidgetEventFilter::widgetChanged, this,
                         [=, this](QObject *obj){
                            auto metaobj = obj->metaObject();
                            auto prop_count = metaobj->propertyCount();
                            for(auto i = 0; i < prop_count; i++) {
                                auto prop = metaobj->property(i);
                                auto varprop_it = propMap.find(prop.name());
                                if(varprop_it != propMap.end()) {
                                    auto read_prop = prop.read(obj);
                                    varprop_it.value()->setValue(read_prop);
                                    qDebug() << read_prop;
                                }
                            }
                         });


            //customplot->setProperty(const char *name, const QVariant &value)


            auto metaobj = customplot->metaObject();
            auto prop_count = metaobj->propertyCount();
            QStringList properties;
            auto j = 0;
            for(auto i = 0; i < prop_count; i++) {
                auto prop = metaobj->property(i);
                auto read_prop = prop.read(customplot);
                properties << QString::fromLatin1(prop.name());
                if(read_prop.isValid() && !read_prop.isNull()) {
                    if(read_prop.type() > 65000) {
                        continue;
                    }
                    auto added_props = variantManager->addProperty(read_prop.type(),prop.name());
                    if(added_props) {
                        added_props->setValue(std::move(read_prop));
                        qDebug() << "Added property: " << prop.name() << "/" << read_prop.typeId() << "read_prop" << read_prop;
                        auto item = browser->addProperty(added_props);
                        item->property()->setEnabled(prop.isWritable());
                        propMap.insert(prop.name(),added_props);
                    } else {
                        qDebug() << "Unable to add property: " << prop.name() << "/" << read_prop.typeId() << "/" << read_prop;
                        j++;
                    }
                }
                else {
                    qDebug() << "Is invalid or null:" << read_prop;
                }
            }
            qDebug() << j << "/"<< prop_count;
            qDebug() << properties;
            // dynamicPropertyNames() 
           
            auto qcustomplotPlotPropertyDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="qcustomplot-properties"});
            qcustomplotPlotPropertyDock->setWidget(browser);
        }
        //{
        //    auto imguiPlotDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="imgui-plot"});
        //    imguiPlotDock->setWidget(new QAdvImguiPlot(&mainWindow));
        //}
        mainWindow.show();
    }
    ~QtAdvPlot_App() noexcept override {
        delete variantManager;
        delete browser;
        delete variantEditor;
    };
};

int main(int argc, char *argv[])
{
    QtAdvPlot_App app(argc,argv);

    return app.exec();
}
