#include <qmetaobject.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qopenglextrafunctions.h>
#include <qopenglwidget.h>
#include <qproperty.h>
#include <qsize.h>

#include <QMetaObject>
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QProperty>
#include <QMap>

#include <DockManager.h>
#include <qtpropertybrowser.h>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>

#include "qt-adv-plot.hpp"

#include "qadv_plot.hpp"
#include "qadv_imgui.hpp"
#include "qadv_qcustomplot.hpp"

#include "widgeteventfilter.hpp"

#include <QtTreePropertyBrowser>
#include <QtVariantEditorFactory>
#include <QtVariantProperty>
#include "qadv_propertybrowser.hpp"
#include <QPropertyChangeHandler>


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

static QMap<QtProperty * , QObject * > recursiveProperties(QtVariantPropertyManager * manager, QObject* from, QtAbstractPropertyBrowser * browser, QtProperty * parent_property = nullptr, const QMetaObject * meta_obj = nullptr) {

    QMap<QtProperty * , QObject * > ret;

    if(!meta_obj) { 
        meta_obj =  from->metaObject();
        auto dyn_prop = from->dynamicPropertyNames();
        qDebug() << "Dyn. Properties: \n" << dyn_prop;
    };
    auto name = meta_obj->className();
    //if(name.isEmpty() || from->|| from->inherits(meta_obj->className())) name = meta_obj->className();
    //else name.append(" (T=").append(meta_obj->className()).append(")");

    bool is_parent = false;
    QtProperty * top_property;
    if(!parent_property) {
        auto parent_name = from->objectName(); 
        parent_property = manager->addProperty(QtVariantPropertyManager::groupTypeId(), name);
        static_cast<QtVariantProperty*>(parent_property)->setValue(parent_name);
        parent_property->setWhatsThis(from->objectName());
        ret[parent_property] = from;
        is_parent = true;
        top_property = parent_property;
    } else {
        top_property = manager->addProperty(QtVariantPropertyManager::groupTypeId(), name);
    }

    if(auto supermeta = meta_obj->superClass()) recursiveProperties(manager, from, browser, top_property, supermeta);

    qDebug() << "Classname: " << meta_obj->className(); 
   // qDebug() << "Properties: ";
   // QStringList properties;
    QStringList unsupported_properties;
    for(auto i = meta_obj->propertyOffset(); i < meta_obj->propertyCount(); i++) {
        auto meta_property = meta_obj->property(i);
     //   QString info(QString::fromLatin1(meta_property.name()));
     //   info.append("[").append(meta_property.typeName());
        QVariant property;
        if(meta_property.isReadable()) {
            property = meta_property.read(from);
           // info.append("|").append(property.toString());
            if(meta_property.isBindable())
            {
               // info.append("|bindable");
            }
        } else {
          // info.append("|!read");
        }
        if(meta_property.isWritable()) {
           // info.append("|write");
        }
       // info.append("]");
       // info.append("{ID:").append(QString::number(property.typeId()));
        if(meta_property.isReadable()) {
            auto manager_property = manager->addProperty(property.typeId(),meta_property.name());
            if(manager_property)  {
                manager_property->setEnabled(meta_property.isWritable());
                manager_property->setValue(property);
                top_property->addSubProperty(manager_property);
                ret[manager_property] = from;
            } else {
              //  info.append("|!added");
                auto not_added_info = QStringLiteral("Name: %0|Type: %1").arg(QString::fromLatin1(meta_property.name()), QString::fromLatin1(meta_property.typeName()));
                unsupported_properties << not_added_info;
            }
        }
      //  info.append("}");
      //  properties << info;
    }

    if(is_parent)
    {
        browser->addProperty(parent_property);
    } else {
        parent_property->addSubProperty(top_property);
    }

    //qDebug() << properties;
    qDebug() << "Unsupported: " << unsupported_properties;

    return ret;
}

static void recursive_do_not_expand(QtTreePropertyBrowser * brow, const QList<QtBrowserItem *> &items) {
    for(auto item : items) {
        brow->setExpanded(item, false);
        recursive_do_not_expand(brow, item->children());
    }
}

class QtAdvPlot_App : public QApplication {
private:
    QMainWindow mainWindow;
    ads::CDockManager* m_DockManager;

    QAdvCustomPlot * customplot;

    QtVariantPropertyManager  *variantManager;
    QtVariantEditorFactory *variantEditor;
    QtTreePropertyBrowser *browser;
    //WidgetEventFilter *filter;
    QList<QtVariantPropertyManager *> variant_managers;

    QMap<QString, QtVariantProperty*> propMap;
    std::vector<PropertyNotifyBinder*> prop_connectors;
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
            customplot->setObjectName("CustomPlot");

            connect(customplot->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot->xAxis2, SLOT(setRange(QCPRange)));
            connect(customplot->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot->yAxis2, SLOT(setRange(QCPRange)));

            auto qcustomplotPlotDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="qcustomplot-plot"});
            qcustomplotPlotDock->setWidget(customplot);

            variantEditor = new QAdv_VariantEditorFactory(&mainWindow);//QtVariantEditorFactory QAdv_VariantEditorFactory(&mainWindow);
            browser = new QtTreePropertyBrowser(&mainWindow);
            variantManager = new QAdv_VariantPropertyManager(&mainWindow);//QtVariantPropertyManager QAdv_VariantPropertyManager(&mainWindow);
            browser->setFactoryForManager(variantManager, variantEditor);



            QMap<QtProperty*, QObject *> propToObj;

            {
                auto display_item = customplot;
                propToObj = recursiveProperties(variantManager, display_item, browser);
            }
            {
                auto display_item = customplot->xAxis;
                display_item->setObjectName("xAxisBottom");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->xAxis2;
                display_item->setObjectName("xAxisTop");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->yAxis;
                display_item->setObjectName("yAxisLeft");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->yAxis2;
                display_item->setObjectName("yAxisRight");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->legend;
                display_item->setObjectName("legend");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->xAxis->grid();
                display_item->setObjectName("xGridBottom");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->yAxis->grid();
                display_item->setObjectName("yGridLeft");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->xAxis2->grid();
                display_item->setObjectName("xGridTop");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->yAxis2->grid();
                display_item->setObjectName("yGridRight");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }
            {
                auto display_item = customplot->plotLayout();
                display_item->setObjectName("plotLayout");
                propToObj.insert(recursiveProperties(variantManager, display_item, browser));
            }

            connect(variantManager, &QtVariantPropertyManager::valueChanged, this,
                [=,this](QtProperty * prop, const QVariant & variant){
                    if(propToObj.contains(prop))
                    {
                        auto owner = propToObj[prop];
                        QtVariantProperty * property = static_cast<QtVariantProperty*>(prop);
                        owner->setProperty(property->propertyName().toUtf8().data(), variant);
                        customplot->replot();
                    }
                });
            browser->setIndentation(10);
            recursive_do_not_expand(browser, browser->topLevelItems());

            auto qcustomplotPlotPropertyDock = setUpNewDockWidget(mainWindow, *m_DockManager, *viewsmenu,{.title="qcustomplot-properties"});
            qcustomplotPlotPropertyDock->setWidget(browser);
        }
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
