#include "shaderbuilder.h"

#include "shaderutils.h"

#include <QRegularExpression>
#include <QDebug>
#include <QMetaProperty>
#include <QQmlEngine>
#include <QQmlFile>
#include <QFile>
#include <QColor>

#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>

using Qt3DRender::QTexture2D;

ShaderBuilder::ShaderBuilder(QNode *parent)
    : QNode(parent)
{
}

ShaderBuilder::~ShaderBuilder()
{
}

void ShaderBuilder::disconnectDependencies()
{
    for(const auto& node : m_dependencies) {
        disconnect(node, &ShaderNode::headerChanged, this, &ShaderBuilder::markDirty);
        disconnect(node, &ShaderNode::resultChanged, this, &ShaderBuilder::markDirty);
        disconnect(node, &ShaderNode::sourceChanged, this, &ShaderBuilder::markDirty);
        disconnect(node, &ShaderNode::typeChanged, this, &ShaderBuilder::markDirty);
        disconnect(node, &ShaderNode::identifierChanged, this, &ShaderBuilder::markDirty);
        disconnect(node, &ShaderNode::propertyTypeChanged, this, &ShaderBuilder::markDirty);
        disconnect(node, &ShaderNode::markDirty, this, &ShaderBuilder::markDirty);
    }
}

void ShaderBuilder::clear()
{
    emit clearBegin();

    if(m_renderPass) {
        for(ShaderUniformValue *uniform : m_uniforms) {
            m_renderPass->removeParameter(uniform->m_parameter);
        }
    }

    disconnectDependencies();

    m_dependencies.clear();

    m_uniforms.clear();
    m_finalShader = "";
}

QRenderPass *ShaderBuilder::renderPass() const
{
    return m_renderPass;
}

QString ShaderBuilder::source() const
{
    return m_source;
}

QString ShaderBuilder::finalShader()
{
    if(m_dirty) {
        rebuildShader();
    }
    return m_finalShader;
}

QQmlListProperty<ShaderNode> ShaderBuilder::inputs()
{
    return QQmlListProperty<ShaderNode>(this, 0,
                                          ShaderBuilder::appendInput,
                                          ShaderBuilder::inputCount,
                                          ShaderBuilder::inputAt,
                                          ShaderBuilder::clearInputs);
}

QQmlListProperty<ShaderOutput> ShaderBuilder::outputs()
{
    return QQmlListProperty<ShaderOutput>(this, 0,
                                          ShaderBuilder::appendOutput,
                                          ShaderBuilder::outputCount,
                                          ShaderBuilder::outputAt,
                                          ShaderBuilder::clearOutputs);
}

void ShaderBuilder::addUniform(ShaderUniformValue *uniform)
{
    m_uniforms.append(uniform);
}

void ShaderBuilder::appendOutput(QQmlListProperty<ShaderOutput> *list, ShaderOutput *output)
{
    if(!output) {
        return;
    }

    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    self->m_outputs.append(output);
    QObject::connect(output, &ShaderOutput::nameChanged, self, &ShaderBuilder::markDirty);
    QObject::connect(output, &ShaderOutput::typeChanged, self, &ShaderBuilder::markDirty);
}

int ShaderBuilder::outputCount(QQmlListProperty<ShaderOutput> *list)
{
    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    return self->m_outputs.count();
}

ShaderOutput *ShaderBuilder::outputAt(QQmlListProperty<ShaderOutput> *list, int index)
{
    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    return self->m_outputs.at(index);
}

void ShaderBuilder::clearOutputs(QQmlListProperty<ShaderOutput> *list)
{
    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    for(const ShaderOutput *output : self->m_outputs) {
        QObject::disconnect(output, &ShaderOutput::nameChanged, self, &ShaderBuilder::markDirty);
        QObject::disconnect(output, &ShaderOutput::typeChanged, self, &ShaderBuilder::markDirty);
    }
    return self->m_outputs.clear();
}

void ShaderBuilder::appendInput(QQmlListProperty<ShaderNode> *list, ShaderNode *input)
{
    if(!input) {
        return;
    }

    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    self->m_inputs.append(input);
}

int ShaderBuilder::inputCount(QQmlListProperty<ShaderNode> *list)
{
    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    return self->m_inputs.count();
}

ShaderNode *ShaderBuilder::inputAt(QQmlListProperty<ShaderNode> *list, int index)
{
    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    return self->m_inputs.at(index);
}

void ShaderBuilder::clearInputs(QQmlListProperty<ShaderNode> *list)
{
    ShaderBuilder *self = static_cast<ShaderBuilder*>(list->object);
    return self->m_inputs.clear();
}

QUrl ShaderBuilder::sourceFile() const
{
    return m_sourceFile;
}

ShaderBuilder::ShaderType ShaderBuilder::shaderType() const
{
    return m_shaderType;
}

void ShaderBuilder::rebuildShader()
{
    clear();
    m_dirty = false;

    // Verify all that all outputs have values
    for(ShaderOutput *output : m_outputs) {
        const auto& result = output->node()->setup(this);
        if(!result.m_ok) {
            qWarning() << "ERROR: ShaderBuilder::rebuildShader(): One of the shader nodes failed during setup.";
            clear();
            return;
        }
        m_dependencies.insert(output->node());
        m_dependencies.unite(result.m_dependencies);
    }

    for(const auto& node : m_dependencies) {
        connect(node, &ShaderNode::headerChanged, this, &ShaderBuilder::markDirty);
        connect(node, &ShaderNode::resultChanged, this, &ShaderBuilder::markDirty);
        connect(node, &ShaderNode::sourceChanged, this, &ShaderBuilder::markDirty);
        connect(node, &ShaderNode::typeChanged, this, &ShaderBuilder::markDirty);
        connect(node, &ShaderNode::identifierChanged, this, &ShaderBuilder::markDirty);
        connect(node, &ShaderNode::propertyTypeChanged, this, &ShaderBuilder::markDirty);
        connect(node, &ShaderNode::markDirty, this, &ShaderBuilder::markDirty);
    }

    if(m_renderPass) {
        for(const ShaderUniformValue *uniform : m_uniforms) {
            m_renderPass->addParameter(uniform->m_parameter);
        }
    }

    QString header = "";
    header += "\n// ------  begin generated header  ------\n\n";
    for(ShaderOutput *output : m_outputs) {
        header += output->node()->generateHeader();
    }
    header += "\n// ------          uniforms        ------\n\n";
    for(const ShaderUniformValue *uniform : m_uniforms) {
        header += "uniform highp " + uniform->m_type + " " + uniform->m_identifier + ";\n";
    }
    header += "\n// ------         parameters        ------\n\n";
    for(const auto& param : m_shaderParameters) {
        header += "uniform highp " + param.first + " " + param.second + ";\n";
    }
    header += "\n// ------   end generated header   ------\n\n";

    QString body = "";
    body += "\n// ------   begin generated body   ------\n\n";
    for(ShaderOutput *output : m_outputs) {
        body += output->node()->generateBody();
    }
    body += "\n// ------    output assignments    ------\n\n";
    for(ShaderOutput *output : m_outputs) {
        body += output->name()
                + " = "
                + ShaderUtils::convert(output->node()->type(),
                                       output->type(),
                                       output->node()->identifier())
                + ";\n";
    }
    body += "\n// ------    end generated body    ------\n\n";
    body.replace(QRegularExpression("\n"), "\n    ");

    QString contents = m_source;
    contents.replace(QRegularExpression("^#pragma shadernodes header",
                                        QRegularExpression::MultilineOption),
                     header);
    contents.replace(QRegularExpression("^#pragma shadernodes body",
                                        QRegularExpression::MultilineOption),
                     body);

    for(ShaderOutput *output : m_outputs) {
        ShaderNode *value = output->node();
        if(!value) {
            continue;
        }
        value->reset();
    }

    m_finalShader = contents;

    emit buildFinished();
}

//void ShaderBuilder::updateUniform(int i)
//{
//    UniformValue &uniform = m_uniforms[i];
//    QByteArray propertyNameArray = uniform.propertyName.toUtf8();
//    QVariant value = uniform.node->property(propertyNameArray.constData());;
//    uniform.value = value;
//    if(value.type() == QVariant::String) {
//        uniform.parameter->setValue(QColor(value.toString()));
//    } else {
//        uniform.parameter->setValue(value);
//    }
//    QString type = glslType(value);
//    if(type != uniform.type) {
//        uniform.type = type;
//        markDirty();
//    }
//}

void ShaderBuilder::setSourceFile(QUrl sourceFile)
{
    if (m_sourceFile == sourceFile)
        return;

    m_sourceFile = sourceFile;

    QString fileName = QQmlFile::urlToLocalFileOrQrc(sourceFile);
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    setSource(file.readAll());
    markDirty();
    emit sourceFileChanged(sourceFile);
}

void ShaderBuilder::setShaderType(ShaderBuilder::ShaderType shaderType)
{
    if (m_shaderType == shaderType)
        return;

    m_shaderType = shaderType;
    markDirty();
    emit shaderTypeChanged(shaderType);
}

void ShaderBuilder::setRenderPass(QRenderPass *renderPass)
{
    if (m_renderPass == renderPass)
        return;

    clear();
    m_renderPass = renderPass;
    markDirty();
    emit renderPassChanged(renderPass);
}

void ShaderBuilder::setSource(QString source)
{
    if (m_source == source)
        return;
    m_source = source;
    markDirty();
    emit sourceChanged(source);
}

void ShaderBuilder::markDirty()
{
    m_dirty = true;
    emit finalShaderChanged();
}

QString ShaderBuilder::glslType(QVariant value) const
{
    return ShaderUtils::glslType(value);
}
