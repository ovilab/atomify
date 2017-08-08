#include "shaderoutput.h"
#include "shaderutils.h"

#include <QVector3D>

ShaderOutput::ShaderOutput(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
    , m_node(this)
{
}

QString ShaderOutput::name() const
{
    return m_name;
}

QString ShaderOutput::type() const
{
    return m_type;
}

QVariant ShaderOutput::value() const
{
    return m_value;
}

ShaderNode *ShaderOutput::node()
{
    return &m_node;
}

void ShaderOutput::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}

void ShaderOutput::setType(QString type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged(type);
}

void ShaderOutput::setValue(const QVariant &value)
{
    if (m_value == value)
        return;

    m_node.setValue(value);

    m_value = value;
    emit valueChanged(value);
}

ShaderOutputNode::ShaderOutputNode(Qt3DCore::QNode *parent) :
    ShaderNode(parent)
{
    setName("outputnode");
    setResult("$value");
}

QVariant ShaderOutputNode::value() const
{
    return m_value;
}

void ShaderOutputNode::setValue(QVariant value)
{
    if (m_value == value)
        return;

    ShaderNode *oldNode = qvariant_cast<ShaderNode*>(m_value);
    if(oldNode) {
        disconnect(oldNode, &ShaderNode::typeChanged, this, &ShaderOutputNode::setType);
    }

    m_value = value;

    ShaderNode *node = qvariant_cast<ShaderNode*>(value);
    if(node) {
        setType(node->type());
        connect(node, &ShaderNode::typeChanged, this, &ShaderOutputNode::setType);
    }
    else {
        setType(ShaderUtils::glslType(value));
    }

    emit valueChanged(value);
}
