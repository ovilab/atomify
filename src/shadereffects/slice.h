#ifndef SLICE_H
#define SLICE_H

#include "../core/shadereffect.h"

class Slice : public ShaderEffect
{
    Q_OBJECT
    Q_PROPERTY(double distance READ distance WRITE setDistance NOTIFY distanceChanged)
    Q_PROPERTY(QVector3D normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(QVector3D origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(double width READ width WRITE setWidth NOTIFY widthChanged)

    QVector3D m_normal;
    QVector3D m_origin;
    double m_distance = 0.0;
    double m_width = 1.0;

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

    QVector3D origin() const
    {
        return m_origin;
    }

public slots:
    void setDistance(double distance)
    {
        if (m_distance == distance)
            return;
        qDebug() << "New distance: " << distance;
        m_distance = distance;
        emit distanceChanged(distance);
    }
    void setNormal(QVector3D normal)
    {
        if (m_normal == normal)
            return;

        qDebug() << "New normal: " << normal;
        m_normal = normal;
        emit normalChanged(normal);
    }

    void setWidth(double width)
    {
        if (m_width == width)
            return;
        qDebug() << "New width: " << width;
        m_width = width;
        emit widthChanged(width);
    }

    void setOrigin(QVector3D origin)
    {
        if (m_origin == origin)
            return;
        qDebug() << "New origin: " << origin;
        m_origin = origin;
        emit originChanged(origin);
    }

signals:
    void distanceChanged(double distance);
    void normalChanged(QVector3D normal);
    void widthChanged(double width);
    void originChanged(QVector3D origin);
};

#endif // SLICE_H
