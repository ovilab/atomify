#include "periodiccopies.h"

PeriodicCopies::PeriodicCopies()
{

}



QString PeriodicCopies::fragmentShaderDefines()
{
    return QString("");
}

QString PeriodicCopies::vertexShaderDefines()
{
    return QString("");
}

QString PeriodicCopies::geometryShaderDefines()
{
    QString defines = "\n#define ADDPERIODICCOPIES\n";
    defines.append("uniform vec3 systemSize; \n");

    return defines;
}

QString PeriodicCopies::fragmentShaderLibrary()
{
    return QString("");
}

QString PeriodicCopies::vertexShaderLibrary()
{
    return QString("");
}

PeriodicCopies *PeriodicCopies::clone()
{
    PeriodicCopies *clone = new PeriodicCopies();
    clone->copyState(this);

    return clone;
}

void PeriodicCopies::copyState(ShaderEffect *source)
{
    PeriodicCopies *periodicCopies = qobject_cast<PeriodicCopies *>(source);
    m_systemSize = periodicCopies->systemSize();
    m_shadersDirty = periodicCopies->shadersDirty();
    m_enabled = periodicCopies->enabled();
}

void PeriodicCopies::beforeRendering(QOpenGLShaderProgram &shaderProgram)
{
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("systemSize")), m_systemSize);
}

void PeriodicCopies::afterRendering(QOpenGLShaderProgram &shaderProgram)
{
    Q_UNUSED(shaderProgram);
}

QVector3D PeriodicCopies::systemSize() const
{
    return m_systemSize;
}

void PeriodicCopies::setSystemSize(QVector3D systemSize)
{
    if (m_systemSize == systemSize)
        return;

    m_systemSize = systemSize;
    emit systemSizeChanged(systemSize);
}
