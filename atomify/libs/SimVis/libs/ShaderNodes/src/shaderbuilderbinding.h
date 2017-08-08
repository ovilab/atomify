#ifndef SHADERBUILDERBINDING_H
#define SHADERBUILDERBINDING_H

#include "shadernode.h"

#include <QObject>

#include <Qt3DCore/QNode>

class ShaderBuilderBinding : public ShaderNode
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName NOTIFY propertyChanged)
    Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)

public:
    explicit ShaderBuilderBinding(Qt3DCore::QNode *parent = 0);

    virtual ShaderNodeSetupResult setup(ShaderBuilder *shaderBuilder, QString tempIdentifier = QString()) override;
    QVariant defaultValue() const;
    QString propertyName() const;

signals:
    void propertyChanged(QString propertyName);
    void defaultValueChanged(QVariant defaultValue);

public slots:
    void setPropertyName(QString propertyName);
    void setDefaultValue(QVariant defaultValue);

private:
    QString m_property;
    QVariant m_defaultValue = 0.0;
};

#endif // SHADERBUILDERBINDING_H
