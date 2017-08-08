#ifndef NODEWRAPPER_H
#define NODEWRAPPER_H

#include <QObject>
#include <private/qqmlbinding_p.h>

#include "shadernode.h"

class NodeWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ShaderNode* shaderNode READ shaderNode WRITE setShaderNode NOTIFY shaderNodeChanged)

public:
    explicit NodeWrapper(QObject *parent = 0);
    ShaderNode* shaderNode() const;

signals:

    void shaderNodeChanged(ShaderNode* shaderNode);

public slots:
    void setShaderNode(ShaderNode* shaderNode);
    void resetProperty(const QString &propertyName);
//private slots:
//    void handlePropertyChange(const QString &text);
private:
    ShaderNode* m_shaderNode;
    QMap<QString, QQmlAbstractBinding::Ptr> m_resetBindingHash;
    QMap<QString, QVariant> m_resetValueHash;
//    QSignalMapper m_signalMapper;
};

#endif // NODEWRAPPER_H
