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

bool QAdv_VariantPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if ( propertyType == qMetaTypeId<QCPAxis>() ||
         propertyType == qMetaTypeId<QCPLegend>() ||
         propertyType == qMetaTypeId<QCPAxisTicker>()
       )
        return true;
    return QtVariantPropertyManager::
        isPropertyTypeSupported(propertyType);
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

QWidget * QAdv_VariantEditorFactory::createEditor(QtProperty *property, QWidget *parent) {
    
    return QtVariantEditorFactory::createEditor(property,parent);
}


