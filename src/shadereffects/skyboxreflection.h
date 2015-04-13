#ifndef SKYBOXREFLECTION_H
#define SKYBOXREFLECTION_H

#include "../core/shadereffect.h"
#include "../renderables/skybox/skybox.h"

class SkyBoxReflection : public ShaderEffect
{
    Q_OBJECT
    Q_PROPERTY(SkyBox* skybox READ skybox WRITE setSkybox NOTIFY skyboxChanged)
    Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
public:
    SkyBoxReflection();
    ~SkyBoxReflection();

    // ShaderEffect interface
    QString fragmentShaderDefines() override;
    QString vertexShaderDefines() override;
    QString fragmentShaderLibrary() override;
    QString vertexShaderLibrary() override;
    SkyBoxReflection *clone() override;
    void copyState(ShaderEffect *source) override;
    void beforeRendering(QOpenGLShaderProgram &shaderProgram) override;
    void afterRendering(QOpenGLShaderProgram &shaderProgram) override;
    SkyBox* skybox() const;
    float reflectivity() const;

public slots:
    void setSkybox(SkyBox* arg);
    void setReflectivity(float arg);

signals:
    void skyboxChanged(SkyBox* arg);
    void reflectivityChanged(float arg);

private:
    SkyBox* m_skybox = 0;
    float m_reflectivity = 0.2;
};

#endif // SKYBOXREFLECTION_H
