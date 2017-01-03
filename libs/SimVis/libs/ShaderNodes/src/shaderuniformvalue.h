#ifndef SHADERUNIFORMVALUE_H
#define SHADERUNIFORMVALUE_H

class ShaderNode;

#include <Qt3DCore/QNode>
#include <Qt3DRender/QParameter>

class ShaderUniformValue : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    explicit ShaderUniformValue(ShaderNode* parent,
                                const QString& propertyName,
                                const QString& identifier,
                                const QVariant& value);

public:
    ShaderNode *m_node;
    QString m_propertyName;
    QString m_identifier;
    QVariant m_value;
    QString m_type;
    Qt3DRender::QParameter* m_parameter;

public slots:
    void updateValue();

};

#endif // SHADERUNIFORMVALUE_H
