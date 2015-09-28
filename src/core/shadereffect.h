#ifndef SHADEREFFECT_H
#define SHADEREFFECT_H
#include <QObject>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QUuid> // TODO: Use UUID to copy data instead of cloning each time

class ShaderEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
    ShaderEffect();
    ShaderEffect(QUuid uniqueId);
    ~ShaderEffect();
    virtual QString fragmentShaderDefines() = 0;
    virtual QString vertexShaderDefines() = 0;
    virtual QString geometryShaderDefines() { return QString(""); }
    virtual QString fragmentShaderLibrary() = 0;
    virtual QString vertexShaderLibrary() = 0;
    virtual QString geometryShaderLibrary() { return QString(""); }
    virtual ShaderEffect *clone() = 0;
    virtual void copyState(ShaderEffect *source) = 0;
    virtual void beforeRendering(QOpenGLShaderProgram &shaderProgram) = 0;
    virtual void afterRendering(QOpenGLShaderProgram &shaderProgram) = 0;
    bool enabled() const;
    bool shadersDirty() const;
    void setShadersDirty(bool shadersDirty);

public slots:
    void setEnabled(bool arg);

signals:
    void enabledChanged(bool arg);

protected:
    QString contentFromFile(QString fileName);
    bool m_enabled = true;
    bool m_shadersDirty = false;
};

#endif // SHADEREFFECT_H
