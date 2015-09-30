#ifndef CYLINDERS_H
#define CYLINDERS_H

#include "../../core/renderable.h"
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QColor>

class Simulator;

struct CylinderVBOData
{
    QVector3D vertex1;
    QVector3D vertex2;
    float radius1;
    float radius2;
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

    float m_radius = 0.1;
    void uploadVBOs(Cylinders* Cylinders);
    bool m_isInitialized = false;
    int m_vertexCount = 0;
    int m_indexCount = 0;
};

class Cylinders : public Renderable
{
    Q_OBJECT
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)
    Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
public:
    Cylinders(QQuickItem *parent = 0);
    ~Cylinders();

    virtual CylindersRenderer *createRenderer();
    bool dirty() const;
    QVector<CylinderVBOData> &vertices() { return m_vertices; }

    float radius() const
    {
        return m_radius;
    }

public slots:
    void setDirty(bool dirty);

    void setRadius(float radius)
    {
        if (m_radius == radius)
            return;

        m_radius = radius;
        emit radiusChanged(radius);
    }

signals:
    void dirtyChanged(bool dirty);

    void radiusChanged(float radius);

private:
    QVector<CylinderVBOData> m_vertices;

    friend class CylindersRenderer;
    bool m_dirty = false;
    float m_radius = 0.025;
};


#endif // CYLINDERS_H
