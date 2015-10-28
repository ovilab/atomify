#include "slice.h"

Slice::Slice()
{

}



QString Slice::fragmentShaderDefines()
{
    return QString("");
}

QString Slice::vertexShaderDefines()
{
    return QString("");
}

QString Slice::geometryShaderDefines()
{
    return QString("\n#define SLICE\n");
}

QString Slice::fragmentShaderLibrary()
{
    return QString("");
}

QString Slice::vertexShaderLibrary()
{
    return QString("");
}

QString Slice::geometryShaderLibrary()
{
    QString shaderLibrary;
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/slice.glsl"));
    return shaderLibrary;
}

Slice *Slice::clone()
{
    Slice *clone = new Slice();
    clone->copyState(this);

    return clone;
}

void Slice::copyState(ShaderEffect *source)
{
    Slice *slice = qobject_cast<Slice *>(source);
    m_shadersDirty = slice->shadersDirty();
    m_enabled = slice->enabled();
    m_width = slice->width();
    m_normal = slice->normal();
    m_distance = slice->distance();
    m_origo = slice->origo();
    m_systemSize = slice->systemSize();
}

void Slice::beforeRendering(QOpenGLShaderProgram &shaderProgram)
{
    QVector3D normal = m_normal;
    if(normal.length() > 0) normal.normalize();
    QVector3D planeOrigo = m_origo - 0.5*m_systemSize + m_distance*normal;

    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("sliceWidth")), float(m_width));
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("sliceNormal")), normal);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("sliceOrigo")), planeOrigo);
}

void Slice::afterRendering(QOpenGLShaderProgram &shaderProgram)
{
    Q_UNUSED(shaderProgram);
}
