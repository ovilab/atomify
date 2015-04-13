#ifndef SKYBOX_H
#define SKYBOX_H

#include "../../core/renderable.h"
#include <QString>
class QOpenGLTexture;

class SkyBoxRenderer : public RenderableRenderer
{
    Q_OBJECT
public:
    SkyBoxRenderer();
    // RenderableRenderer interface
private:
    bool m_isInitialized = false;
    QOpenGLTexture *m_texture = 0;
    GLuint m_textureId = 0;
    QString m_textureFileBase;
    virtual void beforeLinkProgram();
    virtual void synchronize(Renderable *renderable);
    virtual void render();
    void uploadTexture();
    void uploadVBO();
};

class SkyBox : public Renderable
{
    Q_OBJECT
    Q_PROPERTY(QString texture READ texture WRITE setTexture NOTIFY textureChanged)

    // Renderable interface
public:
    virtual SkyBoxRenderer *createRenderer() override;
    QString texture() const;
    QOpenGLTexture *texturePointer();
public slots:
    void setTexture(QString arg);
signals:
    void textureChanged(QString arg);
private:
    QString m_texture;
    QOpenGLTexture *m_texturePointer = 0;
    friend class SkyBoxRenderer;
};

#endif // SKYBOX_H
