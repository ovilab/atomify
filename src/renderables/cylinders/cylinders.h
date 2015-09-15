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
};

class Cylinders : public Renderable
{
    Q_OBJECT
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)
public:
    Cylinders(QQuickItem *parent = 0);
    ~Cylinders();

    virtual CylindersRenderer *createRenderer();
    bool dirty() const;
    QVector<CylinderVBOData> &vertices() { return m_vertices; }

public slots:
    void setDirty(bool dirty);

signals:
    void dirtyChanged(bool dirty);

private:
    QVector<CylinderVBOData> m_vertices;

    friend class CylindersRenderer;
    bool m_dirty = false;
};


#endif // CYLINDERS_H
