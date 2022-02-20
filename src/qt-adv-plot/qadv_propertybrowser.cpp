#include <qnamespace.h>
#include <qtvariantproperty.h>
#include <memory>
#include <qadv_propertybrowser.hpp>

#include "qcustomplot.h"

static QMap <Qt::BrushStyle,QString> BrushStyleMap {
    {Qt::BrushStyle::NoBrush, "NoBrush"},
    {Qt::BrushStyle::SolidPattern, "SolidPattern"},
    {Qt::BrushStyle::Dense1Pattern, "Dense1Pattern"},
    {Qt::BrushStyle::Dense2Pattern, "Dense2Pattern"},
    {Qt::BrushStyle::Dense3Pattern, "Dense3Pattern"},
    {Qt::BrushStyle::Dense4Pattern, "Dense4Pattern"},
    {Qt::BrushStyle::Dense5Pattern, "Dense5Pattern"},
    {Qt::BrushStyle::Dense6Pattern, "Dense6Pattern"},
    {Qt::BrushStyle::Dense7Pattern, "Dense7Pattern"},
    {Qt::BrushStyle::HorPattern, "HorPattern"},
    {Qt::BrushStyle::VerPattern, "VerPattern"},
    {Qt::BrushStyle::CrossPattern, "CrossPattern"},
    {Qt::BrushStyle::BDiagPattern, "BDiagPattern"},
    {Qt::BrushStyle::FDiagPattern, "FDiagPattern"},
    {Qt::BrushStyle::DiagCrossPattern, "DiagCrossPattern"},
    {Qt::BrushStyle::LinearGradientPattern, "LinearGradientPattern"},
    {Qt::BrushStyle::RadialGradientPattern, "RadialGradientPattern"},
    {Qt::BrushStyle::ConicalGradientPattern, "ConicalGradientPattern"},
    {Qt::BrushStyle::TexturePattern, "TexturePattern"},
};

static QMap <Qt::PenStyle,QString> PenStyleMap {
    {Qt::PenStyle::NoPen, "NoPen"},
    {Qt::PenStyle::SolidLine, "SolidLine"},
    {Qt::PenStyle::DashLine, "DashLine"},
    {Qt::PenStyle::DotLine, "DotLine"},
    {Qt::PenStyle::DashDotLine, "DashDotLine", },
    {Qt::PenStyle::DashDotDotLine, "DashDotDotLine"},
};
static QMap <Qt::PenCapStyle,QString> CapStyleMap {
    {Qt::PenCapStyle::SquareCap, "SquareCap"},
    {Qt::PenCapStyle::FlatCap, "FlatCap"},
    {Qt::PenCapStyle::RoundCap, "RoundCap"}
};

static QMap <Qt::PenJoinStyle,QString> JoinStyleMap {
    {Qt::PenJoinStyle::BevelJoin, "BevelJoin"},
    {Qt::PenJoinStyle::MiterJoin, "MiterJoin"},
    {Qt::PenJoinStyle::RoundJoin, "RoundJoin"},
    {Qt::PenJoinStyle::SvgMiterJoin, "SvgMiterJoin"}
};


QAdv_VariantPropertyManager::QAdv_VariantPropertyManager(QObject *parent) : QtVariantPropertyManager(parent) {
    connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
            this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}
bool QAdv_VariantPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    switch(propertyType) {
    case qMetaTypeId<QPen>() :
    case qMetaTypeId<QBrush>() :
        return true;
    default:
        return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
    }
    // if(propertyType == qMetaTypeId<QCPAxis>()) return true;
    // if(propertyType == qMetaTypeId<QCPLegend>()) return true;
    // if(propertyType == qMetaTypeId<QCPLayer>()) return true;
    // if(propertyType == qMetaTypeId<QCPLayoutGrid>()) return true;
    // if(propertyType == qMetaTypeId<QCPAxisTicker>()) return true;
}


QVariant QAdv_VariantPropertyManager::value(const QtProperty *property) const {
    if (propertyToData.contains(property))
        return propertyToData[property].value;
    return QtVariantPropertyManager::value(property);
}
int QAdv_VariantPropertyManager::valueType(int propertyType) const {
    if(propertyType == QtVariantPropertyManager::groupTypeId()) return qMetaTypeId<QString>();
    switch(propertyType) {
    case qMetaTypeId<QPen>(): return qMetaTypeId<QPen>();
    case qMetaTypeId<QBrush>(): return qMetaTypeId<QBrush>();
    default: return QtVariantPropertyManager::valueType(propertyType);
    }
    // if(propertyType == qMetaTypeId<QCPAxis>()) return qMetaTypeId<QCPAxis>();
    // if(propertyType == qMetaTypeId<QCPLegend>()) return qMetaTypeId<QCPLegend>();
    // if(propertyType == qMetaTypeId<QCPLayer>()) return qMetaTypeId<QCPLayer>();
    // if(propertyType == qMetaTypeId<QCPLayoutGrid>()) return qMetaTypeId<QCPLayoutGrid>();
    // if(propertyType == qMetaTypeId<QCPAxisTicker>()) return qMetaTypeId<QCPAxisTicker>();

}
QString QAdv_VariantPropertyManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        auto value = propertyToData[property].value;
        switch(value.typeId()) {
        case  qMetaTypeId<QPen>(): return "(QPen)";
        case  qMetaTypeId<QBrush>(): return "(QBrush)";
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
            setValue(property,p);
            return;
        }
        case qMetaTypeId<QBrush>(): {
            QBrush p = val.value<QBrush>();
            setValue(property,p);
            return;
        }
        default:
            return;
        }
    }
    QtVariantPropertyManager::setValue(property, val);
}

void QAdv_VariantPropertyManager::setValue(QtProperty *property, const QPen &val)
{
    const QPen& p = val;
    Data d = propertyToData[property];
    if(d.value == p) { return; }
    d.value = p;
    if(d.x["width"]) { d.x["width"]->setValue(p.width()); }
    if(d.x["color"]) { d.x["color"]->setValue(p.color()); }
    if(d.x["style"]) {
         auto index = PenStyleMap.values().indexOf(PenStyleMap[p.style()]);
         d.x["style"]->setValue(index); 
    }
    if(d.x["capstyle"]) {
         auto index = PenStyleMap.values().indexOf(CapStyleMap[p.capStyle()]);
         d.x["capstyle"]->setValue(index); 
    }
    if(d.x["joinstyle"]) {
         auto index = PenStyleMap.values().indexOf(JoinStyleMap[p.joinStyle()]);
         d.x["joinstyle"]->setValue(index); 
    }
    propertyToData[property] = d;
    Q_EMIT propertyChanged(property);
    Q_EMIT valueChanged(property, p);
}

void QAdv_VariantPropertyManager::setValue(QtProperty *property, const QBrush &val)
{
    Data d = propertyToData[property];
    if(d.value == val) { return; }
    d.value = val;
    // if(d.x["width"]) { d.x["width"]->setValue(p.width()); }
    // if(d.x["color"]) { d.x["color"]->setValue(p.color()); }
    // if(d.x["style"]) {
    //      auto index = PenStyleMap.values().indexOf(PenStyleMap[p.style()]);
    //      d.x["style"]->setValue(index); 
    // }
    // if(d.x["capstyle"]) {
    //      auto index = PenStyleMap.values().indexOf(CapStyleMap[p.capStyle()]);
    //      d.x["capstyle"]->setValue(index); 
    // }
    // if(d.x["joinstyle"]) {
    //      auto index = PenStyleMap.values().indexOf(JoinStyleMap[p.joinStyle()]);
    //      d.x["joinstyle"]->setValue(index); 
    // }
    propertyToData[property] = d;
    Q_EMIT propertyChanged(property);
    Q_EMIT valueChanged(property, val);
}

void QAdv_VariantPropertyManager::slotValueChanged(QtProperty *property, const QVariant &value)
{
    //qDebug() << "Searching changed property" << property->propertyName();
    if(dataToProperity.contains(property)) {
        //qDebug() << "Found property";
        auto [string, prop] = dataToProperity[property];
        QVariant v = this->value(prop);
        switch(v.typeId()) {
        case qMetaTypeId<QPen>(): { updateQPen(string, prop, value); break; }
        }
    }
}

void QAdv_VariantPropertyManager::updateQPen(QString &string, QtProperty *prop,const QVariant &value)
{
    QPen p = this->value(prop).value<QPen>();
    if(string.compare("width")==0) { p.setWidth(value.value<int>()); }
    if(string.compare("color")==0) { p.setColor(value.value<QColor>()); }
    if(string.compare("style")==0) { p.setStyle(PenStyleMap.keys().at(value.value<int>())); }
    if(string.compare("capstyle")==0) { p.setCapStyle(CapStyleMap.keys().at(value.value<int>())); }
    if(string.compare("joinstyle")==0) { p.setJoinStyle(JoinStyleMap.keys().at(value.value<int>())); }
    setValue(prop, p);
}

void QAdv_VariantPropertyManager::initializeQPenProperty(QtProperty *property) {
    Data d;
    d.value = QPen();
    QAdv_VariantPropertyManager *that = (QAdv_VariantPropertyManager *)this;
    d.x["width"] = that->addProperty(QMetaType::Type::Int);
    d.x["width"]->setPropertyName(tr("width"));
    property->addSubProperty(d.x["width"]);

    dataToProperity[d.x["width"]] = std::make_tuple("width",property);
    d.x["color"] = that->addProperty(QMetaType::Type::QColor);
    d.x["color"]->setPropertyName(tr("color"));
    property->addSubProperty(d.x["color"]);

    dataToProperity[d.x["color"]] = std::make_tuple("color",property);
    d.x["style"] = that->addProperty(QtVariantPropertyManager::enumTypeId());
    d.x["style"]->setPropertyName(tr("style"));
    d.x["style"]->setAttribute("enumNames", PenStyleMap.values());
    property->addSubProperty(d.x["style"]);

    dataToProperity[d.x["style"]] = std::make_tuple("style", property);
    d.x["capstyle"] = that->addProperty(QtVariantPropertyManager::enumTypeId());
    d.x["capstyle"]->setPropertyName(tr("capstyle"));
    d.x["capstyle"]->setAttribute("enumNames", CapStyleMap.values());
    property->addSubProperty(d.x["capstyle"]);

    dataToProperity[d.x["capstyle"]] = std::make_tuple("capstyle", property);
    d.x["joinstyle"] = that->addProperty(QtVariantPropertyManager::enumTypeId());
    d.x["joinstyle"]->setPropertyName(tr("joinstyle"));
    d.x["joinstyle"]->setAttribute("enumNames", JoinStyleMap.values());
    property->addSubProperty(d.x["joinstyle"]);

    dataToProperity[d.x["joinstyle"]] = std::make_tuple("joinstyle", property);
    propertyToData[property] = d;
}

void QAdv_VariantPropertyManager::initializeQBrushProperty(QtProperty *property) {
    // Data d;
    // d.value = QPen();
    // QAdv_VariantPropertyManager *that = (QAdv_VariantPropertyManager *)this;
    // d.x["width"] = that->addProperty(QMetaType::Type::Int);
    // d.x["width"]->setPropertyName(tr("width"));
    // property->addSubProperty(d.x["width"]);

    // dataToProperity[d.x["width"]] = std::make_tuple("width",property);
    // d.x["color"] = that->addProperty(QMetaType::Type::QColor);
    // d.x["color"]->setPropertyName(tr("color"));
    // property->addSubProperty(d.x["color"]);

    // dataToProperity[d.x["color"]] = std::make_tuple("color",property);
    // d.x["style"] = that->addProperty(QtVariantPropertyManager::enumTypeId());
    // d.x["style"]->setPropertyName(tr("style"));
    // d.x["style"]->setAttribute("enumNames", PenStyleMap.values());
    // property->addSubProperty(d.x["style"]);

    // dataToProperity[d.x["style"]] = std::make_tuple("style", property);
    // d.x["capstyle"] = that->addProperty(QtVariantPropertyManager::enumTypeId());
    // d.x["capstyle"]->setPropertyName(tr("capstyle"));
    // d.x["capstyle"]->setAttribute("enumNames", CapStyleMap.values());
    // property->addSubProperty(d.x["capstyle"]);

    // dataToProperity[d.x["capstyle"]] = std::make_tuple("capstyle", property);
    // d.x["joinstyle"] = that->addProperty(QtVariantPropertyManager::enumTypeId());
    // d.x["joinstyle"]->setPropertyName(tr("joinstyle"));
    // d.x["joinstyle"]->setAttribute("enumNames", JoinStyleMap.values());
    // property->addSubProperty(d.x["joinstyle"]);

    // dataToProperity[d.x["joinstyle"]] = std::make_tuple("joinstyle", property);
    // propertyToData[property] = d;
}

void QAdv_VariantPropertyManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == qMetaTypeId<QPen>()) { initializeQPenProperty(property); }
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

void QAdv_VariantPropertyManager::slotPropertyDestroyed(QtProperty *property)
{
    if (dataToProperity.contains(property)) {
        auto [string, dataProperty] = dataToProperity[property];
        propertyToData[dataProperty].x[string] = nullptr;
        dataToProperity.remove(property);
    }
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

