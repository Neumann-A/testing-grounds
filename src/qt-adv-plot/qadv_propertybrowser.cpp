#include <qtvariantproperty.h>
#include <memory>
#include <qadv_propertybrowser.hpp>

#include "qcustomplot.h"

class QAdv_VariantEditorFactoryPrivate
{
    QAdv_VariantEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QAdv_VariantEditorFactory)
public:
    QMap<QtAbstractEditorFactoryBase *, int> m_factoryToType;
    QMap<int, QtAbstractEditorFactoryBase *> m_typeToFactory;
};

QAdv_VariantPropertyManager::QAdv_VariantPropertyManager(QObject *parent) : QtVariantPropertyManager(parent) {};
bool QAdv_VariantPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if ( propertyType == qMetaTypeId<QCPAxis>() ||
         propertyType == qMetaTypeId<QCPLegend>() ||
         propertyType == qMetaTypeId<QCPAxisTicker>()
       )
        return false;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

QAdv_VariantEditorFactory::QAdv_VariantEditorFactory(QObject *parent) : QtVariantEditorFactory(parent) {
    p_ptr = std::make_unique<QAdv_VariantEditorFactoryPrivate>();
    p_ptr->q_ptr = this;

    //QMap<QtAbstractEditorFactoryBase *, int> m_factoryToType;
    //QMap<int, QtAbstractEditorFactoryBase *> m_typeToFactory;
    //d_ptr->m_spinBoxFactory = new QtSpinBoxFactory(this);
    //d_ptr->m_factoryToType[d_ptr->m_spinBoxFactory] = QVariant::Int;
    //d_ptr->m_typeToFactory[QVariant::Int] = d_ptr->m_spinBoxFactory;

};

QAdv_VariantEditorFactory::~QAdv_VariantEditorFactory () {};

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


