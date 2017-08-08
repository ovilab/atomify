#ifndef SHADEROUTPUT_H
#define SHADEROUTPUT_H

#include "shadernode.h"

#include <QObject>

#include <Qt3DCore/QNode>

class ShaderOutputNode : public ShaderNode
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    ShaderOutputNode(Qt3DCore::QNode *parent = 0);
    QVariant value() const;

public slots:
    void setValue(QVariant value);

signals:
    void valueChanged(QVariant value);

private:
    QVariant m_value;
};

class ShaderOutput : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit ShaderOutput(Qt3DCore::QNode *parent = 0);

    QString name() const;
    QString type() const;
    QVariant value() const;
    ShaderNode* node();

signals:
    void nameChanged(QString name);
    void typeChanged(QString type);
    void valueChanged(QVariant value);

public slots:
    void setName(QString name);
    void setType(QString type);

    void setValue(const QVariant &value);

private:

    QString m_name;
    QString m_type;
    QVariant m_value;
    ShaderOutputNode m_node;
};

#endif // SHADEROUTPUT_H
