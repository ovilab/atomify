#include "simplextexture.h"


QString SimplexTexture::fragmentShaderDefines()
{
    QString defines;

    if(m_enabled) {
        defines.append("\n#define SIMPLEXTEXTURE\n");
        if(m_timeDependent) {
            defines.append("#define TIMEDEPENDENTSIMPLEXTEXTURE\n");
        }
    }

    return defines;
}

QString SimplexTexture::vertexShaderDefines()
{
    QString defines;
    return defines;
}

QString SimplexTexture::fragmentShaderLibrary()
{
    QString shaderLibrary;
    // Remember to include dependencies here
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/simplex3.glsl"));
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/simplex4.glsl"));
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/simplextexture.glsl"));
    return shaderLibrary;
}

QString SimplexTexture::vertexShaderLibrary()
{
    QString shaderLibrary;
    return shaderLibrary;
}

SimplexTexture *SimplexTexture::clone()
{
    SimplexTexture *clone = new SimplexTexture();
    clone->copyState(this);
    return clone;
}

void SimplexTexture::copyState(ShaderEffect *source)
{
    SimplexTexture *simplexTexture = qobject_cast<SimplexTexture*>(source);
    m_scale = simplexTexture->scale();
    m_timeDependent = simplexTexture->timeDependent();

    m_enabled = source->enabled();
    m_shadersDirty = source->shadersDirty();
}

void SimplexTexture::beforeRendering(QOpenGLShaderProgram &shaderProgram)
{
    shaderProgram.setUniformValue("cp_simplexTextureScale", m_scale);
}

void SimplexTexture::afterRendering(QOpenGLShaderProgram &shaderProgram)
{
    Q_UNUSED(shaderProgram);
}

float SimplexTexture::scale() const
{
    return m_scale;
}

bool SimplexTexture::timeDependent() const
{
    return m_timeDependent;
}

void SimplexTexture::setScale(float arg)
{
    if (m_scale == arg)
        return;

    m_scale = arg;
    emit scaleChanged(arg);
}

void SimplexTexture::setTimeDependent(bool arg)
{
    if (m_timeDependent == arg)
        return;

    m_timeDependent = arg;
    m_shadersDirty = true;
    emit timeDependentChanged(arg);
}
