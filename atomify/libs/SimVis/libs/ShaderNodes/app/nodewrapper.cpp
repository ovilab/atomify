#include "nodewrapper.h"

NodeWrapper::NodeWrapper(QObject *parent) : QObject(parent)
{
//    connect(&m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(handlePropertyChange(QString)));
}

ShaderNode *NodeWrapper::shaderNode() const
{
    return m_shaderNode;
}

void NodeWrapper::setShaderNode(ShaderNode *shaderNode)
{
    if (m_shaderNode == shaderNode)
        return;

    m_resetBindingHash.clear();
    m_resetValueHash.clear();

    m_shaderNode = shaderNode;


    if(m_shaderNode) {
        QStringList propertyNames = m_shaderNode->inputNames();
        for(const QString &propertyName : propertyNames) {
            QQmlProperty property(m_shaderNode, propertyName, QQmlEngine::contextForObject(m_shaderNode));
            QQmlAbstractBinding::Ptr binding = QQmlAbstractBinding::Ptr(QQmlPropertyPrivate::binding(property));
            if(binding) {
                m_resetBindingHash[propertyName] = binding;
            } else if (property.isWritable()) {
                m_resetValueHash.insert(propertyName, property.read());
            }
//            if(property.hasNotifySignal()) {
//                m_signalMapper.setMapping(property.object(), propertyName);
//                property.connectNotifySignal(&m_signalMapper, SLOT(map()));
//            }
        }
    }

    emit shaderNodeChanged(shaderNode);
}

//void NodeWrapper::handlePropertyChange(const QString &text)
//{
//    qDebug() << text << m_shaderNode->property(text.toStdString().c_str());
//}

void NodeWrapper::resetProperty(const QString &propertyName)
{
    QQmlProperty property(m_shaderNode, propertyName);
    if(!property.isValid()) {
        return;
    }
    if(m_resetBindingHash.contains(propertyName)) {
        QQmlAbstractBinding *binding = m_resetBindingHash[propertyName].data();
        QQmlBinding *qmlBinding = dynamic_cast<QQmlBinding*>(binding);
        if (qmlBinding)
            qmlBinding->setTarget(property);
        QQmlPropertyPrivate::setBinding(binding);
        if (qmlBinding)
            qmlBinding->update();
    } else if(m_resetValueHash.contains(propertyName)) {
        property.write(m_resetValueHash[propertyName]);
    }
}
