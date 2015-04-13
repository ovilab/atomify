#ifndef SIMPLEXTEXTURE_H
#define SIMPLEXTEXTURE_H
#include "../core/shadereffect.h"

class SimplexTexture : public ShaderEffect
{
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(bool timeDependent READ timeDependent WRITE setTimeDependent NOTIFY timeDependentChanged)
public:
    // ShaderEffect interface
    QString fragmentShaderDefines() override;
    QString vertexShaderDefines() override;
    QString fragmentShaderLibrary() override;
    QString vertexShaderLibrary() override;
    SimplexTexture *clone() override;
    void copyState(ShaderEffect *source) override;
    void beforeRendering(QOpenGLShaderProgram &shaderProgram) override;
    void afterRendering(QOpenGLShaderProgram &shaderProgram) override;

    float scale() const;
    bool timeDependent() const;

public slots:
    void setScale(float arg);
    void setTimeDependent(bool arg);

signals:
    void scaleChanged(float arg);
    void timeDependentChanged(bool arg);

private:
    float m_scale = 1.0;
    bool m_timeDependent = false;
};

#endif // SIMPLEXTEXTURE_H
