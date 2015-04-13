#include "skyboxreflection.h"
#include <QOpenGLTexture>

SkyBoxReflection::SkyBoxReflection()
{

}

SkyBoxReflection::~SkyBoxReflection()
{

}

QString SkyBoxReflection::fragmentShaderDefines()
{
    QString defines;
    if(m_enabled) {
        defines.append("\n#define SKYBOXREFLECTION\n");
    }

    return defines;
}

QString SkyBoxReflection::vertexShaderDefines()
{
    return "";
}

QString SkyBoxReflection::fragmentShaderLibrary()
{
    QString shaderLibrary;
    // Remember to include dependencies here
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/skyboxreflection.glsl"));
    return shaderLibrary;
}

QString SkyBoxReflection::vertexShaderLibrary()
{
    return "";
}

SkyBoxReflection *SkyBoxReflection::clone()
{
    SkyBoxReflection *clone = new SkyBoxReflection();
    clone->setSkybox(m_skybox);
    clone->setReflectivity(m_reflectivity);
    return clone;
}

void SkyBoxReflection::copyState(ShaderEffect *source)
{

}

void SkyBoxReflection::beforeRendering(QOpenGLShaderProgram &shaderProgram)
{
    if(!m_skybox) return;
    m_skybox->texturePointer()->bind();
    shaderProgram.setUniformValue("cp_skyboxReflectivity", m_reflectivity);
}

void SkyBoxReflection::afterRendering(QOpenGLShaderProgram &shaderProgram)
{
    if(!m_skybox) return;
    m_skybox->texturePointer()->release();
}

SkyBox *SkyBoxReflection::skybox() const
{
    return m_skybox;
}

float SkyBoxReflection::reflectivity() const
{
    return m_reflectivity;
}

void SkyBoxReflection::setSkybox(SkyBox *arg)
{
    if (m_skybox == arg)
        return;

    m_skybox = arg;
    emit skyboxChanged(arg);
}

void SkyBoxReflection::setReflectivity(float arg)
{
    if (m_reflectivity == arg)
        return;
    m_reflectivity = arg;
    emit reflectivityChanged(arg);
}
