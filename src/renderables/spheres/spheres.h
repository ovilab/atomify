#ifndef SPHERES_H
#define SPHERES_H

#include "../../core/renderable.h"
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QColor>

class Simulator;

struct SphereVBOData
{
    float sphereId;
    float scale;
    float vertexId;
    QVector3D position;
    QVector3D color;
    QVector2D textureCoord;
};

class Spheres;
class SpheresRenderer : public RenderableRenderer
{
public:
    SpheresRenderer();

private:
    virtual void synchronize(Renderable *) override;
    virtual void render() override;
    virtual void beforeLinkProgram() override;

    void uploadVBOs(Spheres* spheres);
    bool m_isInitialized = false;
    int m_vertexCount = 0;
    int m_indexCount = 0;
    QVector3D m_upVector;
    QVector3D m_viewVector;
    QVector3D m_rightVector;
};

class Spheres : public Renderable
{
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)
public:
    Spheres(QQuickItem *parent = 0);
    ~Spheres();
    void setPositions(QVector<QVector3D> &positions);
    QVector<QVector3D> &positions();
    float scale() const;
    void setScale(float scale);
    QColor color() const;
    void setColor(const QColor &color);
    virtual SpheresRenderer *createRenderer();
    QVector<QColor> &colors();
    void setColors(const QVector<QColor> &colors);
    QVector<float> &scales();
    void setScales(const QVector<float> &scales);
    bool dirty() const;

public slots:
    void setDirty(bool dirty);

signals:
    void scaleChanged(bool arg);
    void colorChanged(QColor arg);
    void dirtyChanged(bool dirty);

private:
    QVector3D vectorFromColor(const QColor &color);
    QVector<SphereVBOData> m_vertices;
    QVector<GLuint> m_indices;
    QVector<QVector3D> m_positions;
    QVector<QColor> m_colors;
    QVector<float> m_scales;
    QColor m_color = QColor(0.8, 0.7, 0.5, 1.0);
    float m_scale = 1.0;
    bool m_dirty = false;
    friend class SpheresRenderer;
};


#endif // SPHERES_H
