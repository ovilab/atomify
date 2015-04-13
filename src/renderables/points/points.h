#pragma once
#ifndef POINTS_H
#define POINTS_H

#include "../../core/renderable.h"

#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QColor>
class Simulator;

class Points;
class PointsRenderer : public RenderableRenderer
{
    Q_OBJECT
public:
    PointsRenderer();
private:
    virtual void synchronize(Renderable *renderable) override;
    virtual void render() override;

    void uploadVBO(Points* points);
    void beforeLinkProgram() override;
    int m_vertexCount = 0;
    float m_pointSize = 10.0;
    QVector4D m_color = QVector4D(1.0, 0.0, 0.0, 0.0);
    bool m_isInitialized = false;
};

class Points : public Renderable
{
    Q_OBJECT
    Q_PROPERTY(float pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    Points();
    ~Points();
    void setPositions(QVector<QVector3D> &positions);
    QVector<QVector3D> &positions();
    virtual RenderableRenderer* createRenderer();

    float pointSize() const;
    QColor color() const;

public slots:

    void setPointSize(float arg);
    void setColor(QColor arg);

signals:

    void pointSizeChanged(float arg);
    void colorChanged(QColor arg);

private:
    QVector<QVector3D> m_vertices;
    float m_pointSize = 100.0;
    QColor m_color = "red";
    friend class PointsRenderer;
};


#endif // POINTS_H
