#ifndef SLICE_H
#define SLICE_H

#include "../core/shadereffect.h"

class Slice : public ShaderEffect
{
    Q_OBJECT
    Q_PROPERTY(double distance READ distance WRITE setDistance NOTIFY distanceChanged)
    Q_PROPERTY(QVector3D normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(QVector3D origo READ origo WRITE setOrigo NOTIFY origoChanged)
    Q_PROPERTY(double width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QVector3D systemSize READ systemSize WRITE setSystemSize NOTIFY systemSizeChanged)

    QVector3D m_normal;
    QVector3D m_origo;
    double m_distance = 0.0;
    double m_width = 1.0;
    QVector3D m_systemSize;

public:
    Slice();

    // ShaderEffect interface
public:
    virtual QString fragmentShaderDefines();
    virtual QString vertexShaderDefines();
    virtual QString geometryShaderDefines();
    virtual QString fragmentShaderLibrary();
    virtual QString vertexShaderLibrary();
    virtual QString geometryShaderLibrary();
    virtual Slice *clone();
    virtual void copyState(ShaderEffect *source);
    virtual void beforeRendering(QOpenGLShaderProgram &shaderProgram);
    virtual void afterRendering(QOpenGLShaderProgram &shaderProgram);
    double distance() const
    {
        return m_distance;
    }
    QVector3D normal() const
    {
        return m_normal;
    }

    double width() const
    {
        return m_width;
    }

    QVector3D origo() const
    {
        return m_origo;
    }

    QVector3D systemSize() const
    {
        return m_systemSize;
    }

public slots:
    void setDistance(double distance)
    {
        if (m_distance == distance)
            return;
        m_distance = distance;
        emit distanceChanged(distance);
    }
    void setNormal(QVector3D normal)
    {
        if (m_normal == normal)
            return;

        m_normal = normal;
        emit normalChanged(normal);
    }

    void setWidth(double width)
    {
        if (m_width == width)
            return;
        m_width = width;
        emit widthChanged(width);
    }

    void setOrigo(QVector3D origo)
    {
        if (m_origo == origo)
            return;

        m_origo = origo;
        emit origoChanged(origo);
    }

    void setSystemSize(QVector3D systemSize)
    {
        if (m_systemSize == systemSize)
            return;

        m_systemSize = systemSize;
        emit systemSizeChanged(systemSize);
    }

signals:
    void distanceChanged(double distance);
    void normalChanged(QVector3D normal);
    void widthChanged(double width);
    void origoChanged(QVector3D origo);
    void systemSizeChanged(QVector3D systemSize);
};

#endif // SLICE_H
