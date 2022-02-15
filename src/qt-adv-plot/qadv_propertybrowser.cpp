#include <qtvariantproperty.h>
#include <memory>
#include <qadv_propertybrowser.hpp>

#include "qcustomplot.h"

static QMap <QString,Qt::PenStyle> PenStyleMap {
    {"NoPen", Qt::PenStyle::NoPen},
    {"SolidLine", Qt::PenStyle::SolidLine},
    {"DashLine", Qt::PenStyle::DashLine},
    {"DotLine", Qt::PenStyle::DotLine},
    {"DashDotLine", Qt::PenStyle::DashDotLine},
    {"DashDotDotLine", Qt::PenStyle::DashDotDotLine},
};

QAdv_VariantPropertyManager::QAdv_VariantPropertyManager(QObject *parent) : QtVariantPropertyManager(parent) {
    connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
            this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}
bool QAdv_VariantPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if(propertyType == qMetaTypeId<QPen>()) return true;
    // if(propertyType == qMetaTypeId<QCPAxis>()) return true;
    // if(propertyType == qMetaTypeId<QCPLegend>()) return true;
    // if(propertyType == qMetaTypeId<QCPLayer>()) return true;
    // if(propertyType == qMetaTypeId<QCPLayoutGrid>()) return true;
    // if(propertyType == qMetaTypeId<QCPAxisTicker>()) return true;

    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}


QVariant QAdv_VariantPropertyManager::value(const QtProperty *property) const {
    if (propertyToData.contains(property))
        return propertyToData[property].value;
    return QtVariantPropertyManager::value(property);
}
int QAdv_VariantPropertyManager::valueType(int propertyType) const {
    if(propertyType == qMetaTypeId<QPen>()) return qMetaTypeId<QPen>();
    // if(propertyType == qMetaTypeId<QCPAxis>()) return qMetaTypeId<QCPAxis>();
    // if(propertyType == qMetaTypeId<QCPLegend>()) return qMetaTypeId<QCPLegend>();
    // if(propertyType == qMetaTypeId<QCPLayer>()) return qMetaTypeId<QCPLayer>();
    // if(propertyType == qMetaTypeId<QCPLayoutGrid>()) return qMetaTypeId<QCPLayoutGrid>();
    // if(propertyType == qMetaTypeId<QCPAxisTicker>()) return qMetaTypeId<QCPAxisTicker>();
    return QtVariantPropertyManager::valueType(propertyType);
}
QString QAdv_VariantPropertyManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        auto value = propertyToData[property].value;
        switch(value.typeId()) {
        case  qMetaTypeId<QPen>():
            return "(QPen)";
        default:
            return "[No Text for Object]";
        }

    }
    return QtVariantPropertyManager::valueText(property);
}

void QAdv_VariantPropertyManager::setValue(QtProperty *property, const QVariant &val)
{
    if (propertyToData.contains(property)) {
        switch(val.typeId()) {
        case qMetaTypeId<QPen>(): {
            QPen p = val.value<QPen>();
            Data d = propertyToData[property];
            d.value = p;
            if(d.x["width"]) { d.x["width"]->setValue(p.width()); }
            if(d.x["color"]) { d.x["color"]->setValue(p.color()); }
            if(d.x["style"]) { d.x["style"]->setValue(PenStyleMap.key(p.style())); }
            propertyToData[property] = d;
            //qDebug() << "setValue(QPen) called: " <<property->propertyName();
            Q_EMIT propertyChanged(property);
            Q_EMIT valueChanged(property, p);
            return;
        }
        default:
            return;
        }
    }
    QtVariantPropertyManager::setValue(property, val);
}

void QAdv_VariantPropertyManager::slotValueChanged(QtProperty *property, const QVariant &value)
{
    qDebug() << "Searching changed property" << property->propertyName();
    if(dataToProperity.contains(property)) {
        qDebug() << "Found property";
        auto [string, prop] = dataToProperity[property];
        QVariant v = this->value(prop);
        switch(v.typeId()) {
            case qMetaTypeId<QPen>(): {
                QPen p = v.value<QPen>();
                if(string.compare("width")==0) { p.setWidth(value.value<int>()); }
                if(string.compare("color")==0) { p.setColor(value.value<QColor>()); }
                if(string.compare("style")==0) { 
                    qDebug() << "Selected Style:"<< value.value<int>() << "/"<< PenStyleMap.keys().at(value.value<int>());
                    p.setStyle(PenStyleMap[PenStyleMap.keys().at(value.value<int>())]); 
                }
                v = p;
            }
        }
        setValue(prop, v);

    }
}

void QAdv_VariantPropertyManager::slotPropertyDestroyed(QtProperty *property)
{
    if (dataToProperity.contains(property)) {
        auto [string, dataProperty] = dataToProperity[property];
        propertyToData[dataProperty].x[string] = nullptr;
        dataToProperity.remove(property);
    }
}

void QAdv_VariantPropertyManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == qMetaTypeId<QPen>()) {
        Data d;
        d.value = QPen();
        QAdv_VariantPropertyManager *that = (QAdv_VariantPropertyManager *)this;
        d.x["width"] = that->addProperty(QMetaType::Type::Int);
        d.x["width"]->setPropertyName(tr("width"));
        property->addSubProperty(d.x["width"]);
        d.p["width"] = property;
        dataToProperity[d.x["width"]] = std::make_tuple("width",property);
        d.x["color"] = that->addProperty(QMetaType::Type::QColor);
        d.x["color"]->setPropertyName(tr("color"));
        property->addSubProperty(d.x["color"]);
        d.p["color"] = property;
        dataToProperity[d.x["color"]] = std::make_tuple("color",property);
        d.x["style"] = that->addProperty(QtVariantPropertyManager::enumTypeId());
        d.x["style"]->setPropertyName(tr("style"));
        qDebug() << "Keys:" <<PenStyleMap.keys();
        d.x["style"]->setAttribute("enumNames", PenStyleMap.keys());
        //d.x["style"]->setValue(1);
        property->addSubProperty(d.x["style"]);
        d.p["style"] = property;
        dataToProperity[d.x["style"]] = std::make_tuple("style", property);
        propertyToData[property] = d;
    }
    QtVariantPropertyManager::initializeProperty(property);
}

void QAdv_VariantPropertyManager::uninitializeProperty(QtProperty *property)
{
    if (propertyToData.contains(property)) {
        Data d = propertyToData[property];
        for(auto &subprops: d.x) {
            dataToProperity.remove(subprops);
        }
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}

// QtVariantProperty *QAdv_VariantPropertyManager::addProperty(int propertyType, const QString &name) {
//     if(propertyType == qMetaTypeId<QPen>()) propertyType = QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPAxis>()) propertyType = QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPLegend>()) propertyType = QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPLayer>()) propertyType = QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPAxisTicker>()) propertyType = QtVariantPropertyManager::groupTypeId();

//     auto retval = QtVariantPropertyManager::addProperty(propertyType, name);

//     if(propertyType == qMetaTypeId<QPen>()) {
//         auto width_prop = QtVariantPropertyManager::addProperty(QMetaType::Type::Int, "width");
//         retval->addSubProperty(width_prop);
//         auto style_prop = QtVariantPropertyManager::addProperty(QtVariantPropertyManager::enumTypeId(), "penstyle");
//         retval->addSubProperty(style_prop);
//         auto capstyle_prop = QtVariantPropertyManager::addProperty(QtVariantPropertyManager::enumTypeId(), "capstyle");
//         retval->addSubProperty(capstyle_prop);
//         auto joinstyle_prop = QtVariantPropertyManager::addProperty(QtVariantPropertyManager::enumTypeId(), "joinstyle");
//         retval->addSubProperty(joinstyle_prop);
//     }

//     return retval;
// };

// int QAdv_VariantPropertyManager::valueType(int propertyType) const {
//     if(propertyType == qMetaTypeId<QPen>()) return QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPAxis>()) return QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPLegend>()) return QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPLayer>()) return QtVariantPropertyManager::groupTypeId();
//     if(propertyType == qMetaTypeId<QCPAxisTicker>()) return QtVariantPropertyManager::groupTypeId();

//     return QtVariantPropertyManager::valueType(propertyType);
// };

QAdv_VariantEditorFactory::QAdv_VariantEditorFactory(QObject *parent) : QtVariantEditorFactory(parent) {
    // connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
    //             this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    // connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
    //             this, SLOT(slotPropertyDestroyed(QtProperty *)));
};

QAdv_VariantEditorFactory::~QAdv_VariantEditorFactory () noexcept {};


QWidget * QAdv_VariantEditorFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    // const int propType = manager->propertyType(property);
    //QtAbstractEditorFactoryBase *factory = d_ptr->m_typeToFactory.value(propType, 0);
    //if (!factory)
    return QtVariantEditorFactory::createEditor(manager,property,parent);
    //return factory->createEditor(QtVariantEditorFactory::wrappedProperty(property), parent);
}

QWidget * QAdv_VariantEditorFactory::createEditor(QtProperty *property, QWidget *parent) {
    // QtAbstractEditorFactoryBase *factory = 0;
    // QtAbstractPropertyManager *manager = property->propertyManager();

    // if (QtAbstractEditorFactoryBase::m_viewToManagerToFactory()->contains(this) &&
    //     (*m_viewToManagerToFactory())[this].contains(manager)) {
    //     factory = (*m_viewToManagerToFactory())[this][manager];
    // }

    // if (!factory)
    //     return 0;
    // return factory->createEditor(property, parent);
    return QtVariantEditorFactory::createEditor(property,parent);
}


#include "moc_qadv_propertybrowser.cpp"

