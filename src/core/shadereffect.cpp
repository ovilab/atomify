#include "shadereffect.h"
#include <QFile>

ShaderEffect::ShaderEffect()
{

}

ShaderEffect::ShaderEffect(QUuid uniqueId)
{
    Q_UNUSED(uniqueId);
}

ShaderEffect::~ShaderEffect()
{

}

bool ShaderEffect::enabled() const
{
    return m_enabled;
}

void ShaderEffect::setEnabled(bool arg)
{
    if (m_enabled == arg)
        return;
    m_enabled = arg;
    m_shadersDirty = true;
    emit enabledChanged(arg);
}

QString ShaderEffect::contentFromFile(QString fileName) {
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Could not open " << f.fileName() << ". Aborting!";
        exit(1);
    }
    if(!f.isOpen()) {
        qDebug() << "Could not open " << f.fileName() << ". Aborting!";
        exit(1);
    }
    QTextStream stream(&f);
    QString content = "\n";
    content.append(stream.readAll());
    content.append("\n");
    f.close();

    return content;
}
bool ShaderEffect::shadersDirty() const
{
    return m_shadersDirty;
}

void ShaderEffect::setShadersDirty(bool shadersDirty)
{
    m_shadersDirty = shadersDirty;
}

