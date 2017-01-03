#include "shaderparameter.h"

ShaderParameter::ShaderParameter(ShaderNode *parent)
    : ShaderNode(parent)
{
    setType("vec4");
}

QString ShaderParameter::generateHeader() const
{
    if(m_hasGeneratedHeader) {
        return QString();
    }
    QString header;
    header += QString("uniform ") + type() + " " + identifier() + ";\n";
    m_hasGeneratedHeader = true;
    return header;
}

QString ShaderParameter::generateBody() const
{
    return QString();
}

ShaderNodeSetupResult ShaderParameter::setup(ShaderBuilder *shaderBuilder, QString tempIdentifier)
{
    Q_UNUSED(shaderBuilder)
    Q_UNUSED(tempIdentifier)
    return {true, QSet<ShaderNode*>()};
}
