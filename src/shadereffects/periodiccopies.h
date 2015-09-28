#ifndef PERIODICCOPIES_H
#define PERIODICCOPIES_H

#include "../core/shadereffect.h"

class PeriodicCopies : public ShaderEffect
{
    Q_OBJECT
    Q_PROPERTY(QVector3D systemSize READ systemSize WRITE setSystemSize NOTIFY systemSizeChanged)
    QVector3D m_systemSize;

public:
    PeriodicCopies();

    // ShaderEffect interface
public:
    virtual QString fragmentShaderDefines();
    virtual QString vertexShaderDefines();
    virtual QString geometryShaderDefines();
    virtual QString fragmentShaderLibrary();
    virtual QString vertexShaderLibrary();
    virtual PeriodicCopies *clone();
    virtual void copyState(ShaderEffect *source);
    virtual void beforeRendering(QOpenGLShaderProgram &shaderProgram);
    virtual void afterRendering(QOpenGLShaderProgram &shaderProgram);
    QVector3D systemSize() const;
public slots:
    void setSystemSize(QVector3D systemSize);
signals:
    void systemSizeChanged(QVector3D systemSize);
};

#endif // PERIODICCOPIES_H
