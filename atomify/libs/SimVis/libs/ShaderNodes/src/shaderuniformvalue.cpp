#include "shaderuniformvalue.h"

#include "shaderutils.h"
#include "shadernode.h"

ShaderUniformValue::ShaderUniformValue(ShaderNode *parent,
                                       const QString &propertyName,
                                       const QString &identifier,
                                       const QVariant &value)
    : Qt3DCore::QNode(parent)
    , m_node(parent)
    , m_propertyName(propertyName)
    , m_identifier(identifier)
    , m_value(value)
    , m_type(ShaderUtils::glslType(m_value))
    , m_parameter(new Qt3DRender::QParameter(identifier, value, this))
{
}

void ShaderUniformValue::updateValue()
{
    QByteArray name = m_propertyName.toUtf8();
    QVariant value = m_node->property(name.constData());
    if(!value.isValid()) {
        // TODO consider throwing a warning
        value = 0.0;
    }
    if(value.type() == QVariant::String) {
        value = QColor(value.toString());
    }
    if(value.type() == QVariant::Int) {
        value = value.toDouble();
    }
    m_value = value;
    m_parameter->setValue(value);
}
