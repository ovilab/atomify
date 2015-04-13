#pragma once
#ifndef BILLBOARDS_H
#define BILLBOARDS_H

#include "../../core/renderable.h"

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QColor>

class Simulator;

struct BillboardVBOData
{
    float billboardId;
    QVector3D position;
    QVector3D color;
    QVector2D textureCoord;
};

class Billboards;
class BillboardsRenderer : public RenderableRenderer
{
public:
    BillboardsRenderer();

private:
    virtual void synchronize(Renderable *) override;
    virtual void render() override;

    void uploadVBOs(Billboards* billboards);

    virtual void beforeLinkProgram() override;
    QOpenGLTexture *uploadTexture(QString texture, QString normalMap);
    QOpenGLTexture *m_texture = 0;
    bool m_isInitialized = false;
    int m_vertexCount = 0;
    int m_indexCount = 0;
    QVector3D m_upVector;
    QVector3D m_viewVector;
    QVector3D m_rightVector;
};

class Billboards : public Renderable
{
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QString texture READ texture WRITE setTexture NOTIFY textureChanged)
    Q_PROPERTY(QString normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)

public:
    Billboards(QQuickItem *parent = 0);
    ~Billboards();
    void setPositions(QVector<QVector3D> &positions);
    QVector<QVector3D> &positions();
    QVector<float> &rotations();
    float scale() const;
    void setScale(float scale);
    QVector3D color() const;
    void setColor(const QColor &color);
    QString texture() const;
    QString normalMap() const;
    virtual RenderableRenderer* createRenderer();

public slots:
    void setTexture(QString arg);
    void setNormalMap(QString arg);

signals:
    void scaleChanged(bool arg);
    void textureChanged(QString arg);
    void normalMapChanged(QString arg);

private:
    QVector3D vectorFromColor(const QColor &color);
    QVector<BillboardVBOData> m_vertices;
    QVector<GLuint> m_indices;
    QVector3D m_color = QVector3D(1.0, 1.0, 1.0);
    QVector<QVector3D> m_positions;
    QVector<float> m_rotations;
    float m_scale = 1.0;
    QString m_texture;
    QString m_normalMap;
    friend class BillboardsRenderer;
};


#endif // BILLBOARDS_H
