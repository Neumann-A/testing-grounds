#pragma once

// https://doc.qt.io/archives/qq/qq18-propertybrowser.html

//#include <QtTreePropertyBrowser>
#include <qtvariantproperty.h>
#include <memory>
#include <QtVariantEditorFactory>
#include <QtVariantPropertyManager>
#include <QtVariantProperty>

class QAdv_VariantEditorFactoryPrivate;

class QAdv_VariantPropertyManager : public QtVariantPropertyManager {
    Q_OBJECT
public:
    QAdv_VariantPropertyManager(QObject *parent);
    QVariant value(const QtProperty *property) const override;
    int valueType(int propertyType) const override;
    bool isPropertyTypeSupported(int propertyType) const override;

    QString valueText(const QtProperty *property) const override;

    Q_SLOT void setValue(QtProperty *property, const QVariant &val) override;
private:
    Q_SLOT void setValue(QtProperty *property, const QPen &val);
    Q_SLOT void slotValueChanged(QtProperty *property, const QVariant &value);
    Q_SLOT void slotPropertyDestroyed(QtProperty *property);
protected:
    void initializeProperty(QtProperty *property) override;
    void uninitializeProperty(QtProperty *property) override;
    //int valueType(int propertyType) const override;
    //QtVariantProperty *addProperty(int propertyType, const QString &name = QString()) override;
private:
    struct Data {
        QVariant value;
        QMap<QString, QtVariantProperty *> x;
        QMap<QString, QtProperty *> p;
    };
    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, std::tuple<QString, QtProperty *>> dataToProperity;
};

class QAdv_VariantEditorFactory : public QtVariantEditorFactory {
public:
    QAdv_VariantEditorFactory(QObject *parent);
    ~QAdv_VariantEditorFactory() noexcept override;
protected:
    QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent) override;
    QWidget *createEditor(QtProperty *property, QWidget *parent) override;
private:
    //std::unique_ptr<QAdv_VariantEditorFactoryPrivate> p_ptr;
};


