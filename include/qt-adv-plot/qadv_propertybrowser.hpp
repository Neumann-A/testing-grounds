#pragma once

// https://doc.qt.io/archives/qq/qq18-propertybrowser.html

//#include <QtTreePropertyBrowser>
#include <memory>
#include <QtVariantEditorFactory>
#include <QtVariantPropertyManager>
#include <QtVariantProperty>

class QAdv_VariantEditorFactoryPrivate;

class QAdv_VariantPropertyManager : public QtVariantPropertyManager {
public:
    QAdv_VariantPropertyManager(QObject *parent);
    bool isPropertyTypeSupported(int propertyType) const override;
};

class QAdv_VariantEditorFactory : public QtVariantEditorFactory {
public:
    QAdv_VariantEditorFactory(QObject *parent);
    ~QAdv_VariantEditorFactory() noexcept override;
protected:
    QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent) override;
    QWidget *createEditor(QtProperty *property, QWidget *parent) override;
private:
    std::unique_ptr<QAdv_VariantEditorFactoryPrivate> p_ptr;
};


