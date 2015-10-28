#include "simplexbump.h"

QString SimplexBump::fragmentShaderDefines()
{
    QString defines;

    if(m_enabled) {
        defines.append("\n#define SIMPLEXBUMP\n");
    }

    return defines;
}

QString SimplexBump::vertexShaderDefines()
{
    QString defines;
    return defines;
}

QString SimplexBump::fragmentShaderLibrary()
{
    QString shaderLibrary;
    // Remember to include dependencies here
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/simplex2.glsl"));
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/simplexbump.glsl"));
    return shaderLibrary;
}

QString SimplexBump::vertexShaderLibrary()
{
    QString shaderLibrary;
    return shaderLibrary;
}

SimplexBump *SimplexBump::clone()
{
    SimplexBump *clone = new SimplexBump();
    clone->copyState(this);

    return clone;
}

void SimplexBump::copyState(ShaderEffect *source)
{
    SimplexBump *simplexBump = qobject_cast<SimplexBump*>(source);
    m_intensity = simplexBump->intensity();
    m_scale = simplexBump->scale();

    m_enabled = source->enabled();
    m_shadersDirty = source->shadersDirty();
}

void SimplexBump::beforeRendering(QOpenGLShaderProgram &shaderProgram)
{
    shaderProgram.setUniformValue("cp_bumpIntensity", m_intensity);
    shaderProgram.setUniformValue("cp_bumpScale", m_scale);
}

void SimplexBump::afterRendering(QOpenGLShaderProgram &shaderProgram)
{
    Q_UNUSED(shaderProgram);
}

float SimplexBump::intensity() const
{
    return m_intensity;
}

float SimplexBump::scale() const
{
    return m_scale;
}

void SimplexBump::setIntensity(float arg)
{
    if (m_intensity == arg)
        return;

    m_intensity = arg;
    emit intensityChanged(arg);
}

void SimplexBump::setScale(float arg)
{
    if (m_scale == arg)
        return;
    m_scale = arg;
    emit scaleChanged(arg);
}

