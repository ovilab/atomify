#ifndef CYLINDERS_H
#define CYLINDERS_H

#include "../../core/renderable.h"
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QColor>

class Simulator;

struct CylinderVBOData
{
    float sphereId;
    QVector3D position;
    QVector3D color;
    QVector2D textureCoord;
};

class Cylinders;
class CylindersRenderer : public RenderableRenderer
{
public:
    CylindersRenderer();

private:
    virtual void synchronize(Renderable *) override;
    virtual void render() override;
    virtual void beforeLinkProgram() override;

    void uploadVBOs(Cylinders* Cylinders);
    bool m_isInitialized = false;
    int m_vertexCount = 0;
    int m_indexCount = 0;
    QVector3D m_upVector;
    QVector3D m_viewVector;
    QVector3D m_rightVector;
};

class Cylinders : public Renderable
{
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)

public:
    Cylinders(QQuickItem *parent = 0);
    ~Cylinders();
    void setPositions(QVector<QVector3D> &positions);
    QVector<QVector3D> &positions();
    float scale() const;
    void setScale(float scale);
    QVector3D color() const;
    void setColor(const QColor &color);
    virtual CylindersRenderer *createRenderer();
signals:
    void scaleChanged(bool arg);

private:
    QVector3D vectorFromColor(const QColor &color);
    QVector<CylinderVBOData> m_vertices;
    QVector<GLuint> m_indices;
    QVector3D m_color = QVector3D(1.0, 1.0, 1.0);
    QVector<QVector3D> m_positions;
    float m_scale = 1.0;

    friend class CylindersRenderer;
};


#endif // CYLINDERS_H
