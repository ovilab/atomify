#ifndef DEFAULTLIGHT_H
#define DEFAULTLIGHT_H

#include "../core/shadereffect.h"

#include <QColor>
#include <QVector3D>

class Light : public ShaderEffect
{
    Q_OBJECT
    Q_PROPERTY(bool ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
    Q_PROPERTY(bool diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(bool specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(QColor ambientColor READ ambientColor WRITE setAmbientColor NOTIFY ambientColorChanged)
    Q_PROPERTY(QColor diffuseColor READ diffuseColor WRITE setDiffuseColor NOTIFY diffuseColorChanged)
    Q_PROPERTY(QColor specularColor READ specularColor WRITE setSpecularColor NOTIFY specularColorChanged)
    Q_PROPERTY(float diffuseIntensity READ diffuseIntensity WRITE setDiffuseIntensity NOTIFY diffuseIntensityChanged)
    Q_PROPERTY(float ambientIntensity READ ambientIntensity WRITE setAmbientIntensity NOTIFY ambientIntensityChanged)
    Q_PROPERTY(float specularIntensity READ specularIntensity WRITE setSpecularIntensity NOTIFY specularIntensityChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
    Q_PROPERTY(float attenuation READ attenuation WRITE setAttenuation NOTIFY attenuationChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)

public:
    // ShaderEffect interface
    QString fragmentShaderDefines() override;
    QString vertexShaderDefines() override;
    QString fragmentShaderLibrary() override;
    QString vertexShaderLibrary() override;
    Light *clone() override;
    void beforeRendering(QOpenGLShaderProgram &shaderProgram) override;
    void afterRendering(QOpenGLShaderProgram &shaderProgram) override;
    void copyState(ShaderEffect *source) override;
    QColor ambientColor() const;
    QColor diffuseColor() const;
    QColor specularColor() const;
    float diffuseIntensity() const;
    float ambientIntensity() const;
    float specularIntensity() const;
    float shininess() const;
    float attenuation() const;
    QVector3D position() const;
    bool ambient() const;
    bool diffuse() const;
    bool specular() const;
    float gamma() const;
    void setLightId(int id);

public slots:
    void setAmbientColor(QColor arg);
    void setDiffuseColor(QColor arg);
    void setSpecularColor(QColor arg);
    void setDiffuseIntensity(float arg);
    void setAmbientIntensity(float arg);
    void setSpecularIntensity(float arg);
    void setShininess(float arg);
    void setAttenuation(float arg);
    void setPosition(QVector3D arg);
    void setAmbient(bool arg);
    void setDiffuse(bool arg);
    void setSpecular(bool arg);
    void setGamma(float arg);

signals:
    void ambientColorChanged(QColor arg);
    void diffuseColorChanged(QColor arg);
    void specularColorChanged(QColor arg);
    void diffuseIntensityChanged(float arg);
    void ambientIntensityChanged(float arg);
    void specularIntensityChanged(float arg);
    void shininessChanged(float arg);
    void attenuationChanged(float arg);
    void positionChanged(QVector3D arg);
    void ambientChanged(bool arg);
    void diffuseChanged(bool arg);
    void specularChanged(bool arg);
    void gammaChanged(float arg);

private:
    QColor m_ambientColor;
    QColor m_diffuseColor;
    QColor m_specularColor;
    QVector3D m_position;
    float m_diffuseIntensity = 1.0;
    float m_ambientIntensity = 0.1;
    float m_specularIntensity = 0.1;
    float m_shininess = 30.0;
    float m_attenuation = 0.01;
    float m_gamma = 2.0;
    int   m_lightId = -1;
    bool m_ambient = true;
    bool m_diffuse = true;
    bool m_specular = true;
};

#endif // DEFAULTLIGHT_H
