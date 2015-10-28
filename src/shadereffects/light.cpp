#include "light.h"

QString Light::fragmentShaderDefines()
{
    QString defines = "\n#define DEFAULTLIGHT \n";
    if(m_ambient) {
        defines.append("#define DEFAULTLIGHTAMBIENT \n");
    }
    if(m_diffuse) {
        defines.append("#define DEFAULTLIGHTDIFFUSE \n");
    }
    if(m_specular) {
        defines.append("#define DEFAULTLIGHTSPECULAR \n");
    }
    return defines;
}

QString Light::vertexShaderDefines()
{
    QString defines = "\n #define DEFAULTLIGHT \n";
    return defines;
}

QString Light::fragmentShaderLibrary()
{
    QString shaderLibrary;
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/light.glsl"));
    return shaderLibrary;
}

QString Light::vertexShaderLibrary()
{
    QString shaderLibrary;
    shaderLibrary.append(contentFromFile(":/org.compphys.SimVis/shadereffects/shaders/light.glsl"));
    return shaderLibrary;
}

void Light::copyState(ShaderEffect *source) {
    Light *defaultLight = qobject_cast<Light*>(source);
    m_ambientColor = defaultLight->ambientColor();
    m_diffuseColor = defaultLight->diffuseColor();
    m_specularColor = defaultLight->specularColor();
    m_shininess = defaultLight->shininess();
    m_attenuation = defaultLight->attenuation();
    m_diffuseIntensity = defaultLight->diffuseIntensity();
    m_specularIntensity = defaultLight->specularIntensity();
    m_ambientIntensity = defaultLight->ambientIntensity();
    m_position = defaultLight->position();
    m_ambient = defaultLight->ambient();
    m_diffuse = defaultLight->diffuse();
    m_specular = defaultLight->specular();
    m_gamma = defaultLight->gamma();

    m_enabled = source->enabled();
    m_shadersDirty = source->shadersDirty();
}

Light *Light::clone()
{
    Light *clone = new Light();
    clone->copyState(this);

    return clone;
}

void Light::beforeRendering(QOpenGLShaderProgram &shaderProgram)
{
    if(m_lightId == -1) {
        qDebug() << "An error occured. A light had light id -1 which indicates that Anders is a bad programmer.";
    }

    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].ambientColor").arg(m_lightId)), m_ambientColor);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].diffuseColor").arg(m_lightId)), m_diffuseColor);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].specularColor").arg(m_lightId)), m_specularColor);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].shininess").arg(m_lightId)), m_shininess);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].attenuation").arg(m_lightId)), m_attenuation);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].gamma").arg(m_lightId)), m_gamma);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].diffuseIntensity").arg(m_lightId)), m_diffuseIntensity);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].specularIntensity").arg(m_lightId)), m_specularIntensity);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].ambientIntensity").arg(m_lightId)), m_ambientIntensity);
    shaderProgram.setUniformValue(shaderProgram.uniformLocation(QString("cp_lights[%1].position").arg(m_lightId)), m_position);

    m_lightId = -1;
}

void Light::afterRendering(QOpenGLShaderProgram &shaderProgram) {
    Q_UNUSED(shaderProgram);
}

QColor Light::ambientColor() const
{
    return m_ambientColor;
}

QColor Light::diffuseColor() const
{
    return m_diffuseColor;
}

QColor Light::specularColor() const
{
    return m_specularColor;
}

float Light::diffuseIntensity() const
{
    return m_diffuseIntensity;
}

float Light::ambientIntensity() const
{
    return m_ambientIntensity;
}

float Light::specularIntensity() const
{
    return m_specularIntensity;
}

float Light::shininess() const
{
    return m_shininess;
}

float Light::attenuation() const
{
    return m_attenuation;
}

QVector3D Light::position() const
{
    return m_position;
}

bool Light::ambient() const
{
    return m_ambient;
}

bool Light::diffuse() const
{
    return m_diffuse;
}

bool Light::specular() const
{
    return m_specular;
}

float Light::gamma() const
{
    return m_gamma;
}

void Light::setLightId(int id)
{
    m_lightId = id;
}

void Light::setAmbientColor(QColor arg)
{
    if (m_ambientColor == arg)
        return;

    m_ambientColor = arg;
    emit ambientColorChanged(arg);
}

void Light::setDiffuseColor(QColor arg)
{
    if (m_diffuseColor == arg)
        return;

    m_diffuseColor = arg;
    emit diffuseColorChanged(arg);
}

void Light::setSpecularColor(QColor arg)
{
    if (m_specularColor == arg)
        return;

    m_specularColor = arg;
    emit specularColorChanged(arg);
}

void Light::setDiffuseIntensity(float arg)
{
    if (m_diffuseIntensity == arg)
        return;

    m_diffuseIntensity = arg;
    emit diffuseIntensityChanged(arg);
}

void Light::setAmbientIntensity(float arg)
{
    if (m_ambientIntensity == arg)
        return;

    m_ambientIntensity = arg;
    emit ambientIntensityChanged(arg);
}

void Light::setSpecularIntensity(float arg)
{
    if (m_specularIntensity == arg)
        return;

    m_specularIntensity = arg;
    emit specularIntensityChanged(arg);
}

void Light::setShininess(float arg)
{
    if (m_shininess == arg)
        return;

    m_shininess = arg;
    emit shininessChanged(arg);
}

void Light::setAttenuation(float arg)
{
    if (m_attenuation == arg)
        return;

    m_attenuation = arg;
    emit attenuationChanged(arg);
}

void Light::setPosition(QVector3D arg)
{
    if (m_position == arg)
        return;

    m_position = arg;
    emit positionChanged(arg);
}

void Light::setAmbient(bool arg)
{
    if (m_ambient == arg)
        return;

    m_ambient = arg;
    m_shadersDirty = true;
    emit ambientChanged(arg);
}

void Light::setDiffuse(bool arg)
{
    if (m_diffuse == arg)
        return;

    m_diffuse = arg;
    m_shadersDirty = true;
    emit diffuseChanged(arg);
}

void Light::setSpecular(bool arg)
{
    if (m_specular == arg)
        return;

    m_specular = arg;
    m_shadersDirty = true;
    emit specularChanged(arg);
}

void Light::setGamma(float arg)
{
    if (m_gamma == arg)
        return;

    m_gamma = arg;
    emit gammaChanged(arg);
}
