#ifndef SHADERBUILDER_H
#define SHADERBUILDER_H

#include "shadernode.h"
#include "shaderoutput.h"

#include <QOpenGLShader>
#include <QObject>
#include <QQmlListProperty>
#include <QUrl>
#include <QVariantList>

#include <Qt3DCore/QNode>
#include <Qt3DRender/QMaterial>

using namespace Qt3DRender;

class ShaderBuilder : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_ENUMS(ShaderType)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl sourceFile READ sourceFile WRITE setSourceFile NOTIFY sourceFileChanged)
    Q_PROPERTY(QString finalShader READ finalShader NOTIFY finalShaderChanged)
    Q_PROPERTY(QQmlListProperty<ShaderNode> inputs READ inputs CONSTANT)
    Q_PROPERTY(QQmlListProperty<ShaderOutput> outputs READ outputs CONSTANT)
    Q_PROPERTY(ShaderType shaderType READ shaderType WRITE setShaderType NOTIFY shaderTypeChanged)
    Q_PROPERTY(Qt3DRender::QRenderPass* renderPass READ renderPass WRITE setRenderPass NOTIFY renderPassChanged)

public:
    enum class ShaderType {
        Vertex,
        Fragment,
        Geometry,
        Compute,
        TessellationControl,
        TessellationEvaluation
    };

    explicit ShaderBuilder(QNode *parent = 0);
    virtual ~ShaderBuilder();

    QString source() const;
    QString finalShader();

    QQmlListProperty<ShaderNode> inputs();
    QQmlListProperty<ShaderOutput> outputs();

    QVariantMap uniforms() const;
    void addUniform(ShaderUniformValue* uniform);

    QUrl sourceFile() const;
    ShaderType shaderType() const;

signals:
    void sourceChanged(QString source);
    void finalShaderChanged();
    void sourceFileChanged(QUrl sourceFile);
    void shaderTypeChanged(ShaderType shaderType);
    void clearBegin();
    void buildFinished();
    void renderPassChanged(Qt3DRender::QRenderPass* renderPass);

public slots:
    void rebuildShader();
    void setSource(QString source);
    void markDirty();
    void setSourceFile(QUrl sourceFile);
    void setShaderType(ShaderType shaderType);
    void setRenderPass(Qt3DRender::QRenderPass* renderPass);

private:
    static void appendOutput(QQmlListProperty<ShaderOutput> *list, ShaderOutput *output);
    static int outputCount(QQmlListProperty<ShaderOutput> *list);
    static ShaderOutput *outputAt(QQmlListProperty<ShaderOutput> *list, int index);
    static void clearOutputs(QQmlListProperty<ShaderOutput> *list);

    static void appendInput(QQmlListProperty<ShaderNode> *list, ShaderNode *input);
    static int inputCount(QQmlListProperty<ShaderNode> *list);
    static ShaderNode *inputAt(QQmlListProperty<ShaderNode> *list, int index);
    static void clearInputs(QQmlListProperty<ShaderNode> *list);

    QString glslType(QVariant value) const;
    void clear();

    QString m_source;
    QList<ShaderOutput*> m_outputs;
    QList<ShaderNode*> m_inputs;
    QUrl m_sourceFile;
    ShaderType m_shaderType = ShaderType::Vertex;
    QString m_finalShader;

    bool m_dirty = true;

    Qt3DRender::QRenderPass* m_renderPass = nullptr;
    QSet<ShaderNode*> m_dependencies;

public:
    QList<QPair<QString, QString>> m_shaderParameters;
    QVector<ShaderUniformValue*> m_uniforms;

    friend class ShaderBuilderMaterialBinding;
    Qt3DRender::QRenderPass* renderPass() const;
    void disconnectDependencies();
};

#endif // SHADERBUILDER_H
