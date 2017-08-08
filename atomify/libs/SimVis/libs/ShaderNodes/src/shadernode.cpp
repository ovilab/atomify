#include "shadernode.h"

#include "shaderparameter.h"
#include "shaderbuilder.h"
#include "shaderutils.h"
#include "shaderbuilderbinding.h"

#include <QDebug>
#include <QJSValueIterator>
#include <QMetaProperty>
#include <QRegularExpression>
#include <QSignalMapper>
#include <QQmlFile>
#include <QFile>

const QStringList builtinNames = QStringList{"objectName","parent","enabled","name","type","result","source","header","identifier","headerFile","requirement","dependencies","data","childNodes","exportedTypeName","arrayProperties", "headerFiles"};

ShaderNode::ShaderNode(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(parent)
{
    setName("unnamed");
}

QString ShaderNode::name() const
{
    return m_name;
}

QString ShaderNode::type() const
{
    return m_type;
}

QString ShaderNode::result() const
{
    return m_result;
}

QString ShaderNode::header() const
{
    return m_header;
}

QString ShaderNode::identifier() const
{
    return m_identifier;
}

QString ShaderNode::generateHeader() const
{
    if(m_hasGeneratedHeader) {
        return QString();
    }

    QString headerResult = "";
    for(const ShaderNode *node : m_resolvedDependencies) {
        headerResult += node->generateHeader();
    }
    if(!m_header.isEmpty()) {
        QString headerIdentifier = "SHADER_NODE_HEADER_" + name().toUpper();
        headerResult += "#ifndef " + headerIdentifier + "\n";
        headerResult += "#define " + headerIdentifier + "\n";
        headerResult += m_header + "\n";
        headerResult += "#endif // " + headerIdentifier + "\n";
    }
    m_hasGeneratedHeader = true;
    return headerResult;
}

QString ShaderNode::generateBody() const
{
    if(m_hasGeneratedBody) {
        return QString();
    }
    if(m_type.isEmpty()) {
        qWarning() << "WARNING: ShaderNode::generateBody(): " << m_name << "is missing type.";
        return QString();
    }
    QString body = "";
    for(ShaderNode* dependency : m_resolvedDependencies) {
        body += dependency->generateBody();
    }
    body += ShaderUtils::precisionQualifier(m_type) + " " + m_type + " " + identifier() + ";\n";
    if(!m_resolvedSource.isEmpty()) {
        body += m_resolvedSource + "\n";
    }
    body += "\n";
    m_hasGeneratedBody = true;
    return body;
}

QString ShaderNode::convert(const QString &targetType, const QString &identifier) const
{
    QString v = identifier;
    if(v.isEmpty()) {
        v = m_identifier;
    }
    return ShaderUtils::convert(type(), targetType, v);
}

void ShaderNode::handlePropertyChange(int index)
{
    if(m_propertyTypeNames.contains(index) && metaObject()->propertyCount() > index) {
        QMetaProperty metaProperty = metaObject()->property(index);
        QVariant value = metaProperty.read(this);
        if(value.type() == QVariant::Int) {
            // Var values may be parsed as int by QML
            // but we need to treat them as floating point numbers
            // TODO make this into a new type to make sure we cannot mistakenly forget to convert it
            // One option is to use phantom types: https://youtu.be/ojZbFIQSdl8?t=24m7s
            value = QVariant(double(value.toInt()));
        }
        QString oldTypeName = m_propertyTypeNames[index];
        QString newTypeName = value.typeName();
        if(oldTypeName != newTypeName) {
            m_propertyTypeNames[index] = newTypeName;
            emit propertyTypeChanged();
        }
        // Mark as dirty for all changes to QJSValues as these cannot easily be checked directly
        if(newTypeName == "QJSValue" || oldTypeName == "QJSValue") {
            emit markDirty();
        }
    }
}

QString ShaderNode::createUniform(const QString &propertyName, const QVariant &value, ShaderBuilder *shaderBuilder)
{
    QByteArray propertyNameArray = propertyName.toUtf8();
    int propertyIndex = metaObject()->indexOfProperty(propertyNameArray.constData());
    if(propertyIndex < 0) {
        qWarning() << "WARNING: ShaderNode::setup(): Could not find shader node or property for "
                   << propertyName << "on" << name();
        return QString("");
    }
    QMetaProperty metaProperty = metaObject()->property(propertyIndex);

    QString propertyNameNoUnderscores = QString(propertyName).replace("_", "");
    QString uniformPrefix = "node_uniform";
    QString targetIdentifier =  uniformPrefix + "_" + propertyNameNoUnderscores + "_" + ShaderUtils::generateName();

    ShaderUniformValue *uniform = new ShaderUniformValue(this, propertyName, targetIdentifier, value);

    if(!metaProperty.hasNotifySignal()) {
        qWarning() << "WARNING: ShaderBuilder::addUniform(): Property" << propertyName << "on" << name()
                   << "has no notification signal.";
    } else {
        const QByteArray signal = '2' + metaProperty.notifySignal().methodSignature();
        connect(this, signal, uniform, SLOT(updateValue()));
    }

    m_uniforms.append(uniform);

    shaderBuilder->addUniform(uniform);

    return targetIdentifier;
}

ShaderNodeSetupResult ShaderNode::setup(ShaderBuilder* shaderBuilder, QString tempIdentifier)
{
    if(m_hasSetup && tempIdentifier.isEmpty()) {
        return {true, m_resolvedDependencies};
    }
    if(!m_requirement) {
        qWarning() << "WARNING: ShaderNode::setup(): Requirement for" << this << name() << "is not satisfied.";
        return {false, QSet<ShaderNode*>()};
    }
    if(m_type.isEmpty()) {
        qWarning() << "WARNING: ShaderNode::setup(): " << name() << "is missing type.";
        return {false, QSet<ShaderNode*>()};
    }
    QString currentIdentifier;
    if(tempIdentifier.isEmpty()) {
        currentIdentifier = identifier();
    } else {
        currentIdentifier = tempIdentifier;
    }

    QString sourceContent;

    sourceContent += m_source;
    if(!m_result.isEmpty()) {
        sourceContent += currentIdentifier + " = " + m_result + ";\n";
    }

    m_resolvedDependencies.clear();

    for(ShaderNode *dependency : m_declaredDependencies) {
        if(dependency == this) {
            continue;
        }
        const auto &dependencySetup = dependency->setup(shaderBuilder);
        if(!dependencySetup.m_ok) {
            return {false, QSet<ShaderNode*>()};
        }
        m_resolvedDependencies.insert(dependency);
        m_resolvedDependencies.unite(dependencySetup.m_dependencies);
    }

    // TODO old signal mappers need to be deleted at some point
    for(QSignalMapper *mapper : m_signalMappers) {
        disconnect(this, 0, mapper, SLOT(map()));
        disconnect(mapper, SIGNAL(mapped(int)), this, SLOT(handlePropertyChange(int)));
        mapper->deleteLater();
    }
    m_signalMappers.clear();

    QStringList propertiesInSource;
    // matches '$property' or '$(property, type)'
    QRegularExpression propertyRegex("\\$(?:\\(\\s*)?([_a-zA-Z0-9]+)\\s*\\)?(?:\\s*,\\s*([_a-zA-Z0-9]+)\\s*\\))?");
    if(!sourceContent.isEmpty()) {
        QRegularExpressionMatchIterator matches = propertyRegex.globalMatch(sourceContent);
        while(matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString propertyName = match.captured(1);
            propertiesInSource.append(propertyName);
        }
    }

    QVariantMap mappings = m_mappings;
    for(int i = 0; i < metaObject()->propertyCount(); i++) {
        QString propertyName = metaObject()->property(i).name();
        if(!propertiesInSource.contains(propertyName)) {
            continue;
        }
        if(mappings.contains(propertyName)) {
            continue;
        }
        QMetaProperty metaProperty = metaObject()->property(i);
        QVariant value = metaProperty.read(this);

        if(!value.isValid()) {
            value = QVariant(0.0);
        }

        if(value.type() == QVariant::Int) {
            // Var values may be parsed as int by QML
            // but we need to treat them as floating point numbers
            value = value.toDouble();
        }
        if(value.type() == QVariant::String) {
            value = QColor(value.toString());
        }

        m_propertyTypeNames[i] = value.typeName();

        if(metaProperty.hasNotifySignal()) {
            QSignalMapper *mapper = new QSignalMapper;
            mapper->setMapping(this, i);
            const QByteArray signal = '2' + metaProperty.notifySignal().methodSignature();
            connect(this, signal, mapper, SLOT(map()));
            connect(mapper, SIGNAL(mapped(int)), this, SLOT(handlePropertyChange(int)));
            m_signalMappers.append(mapper);
        }

        if(!value.isValid()) {
            continue;
        }
        mappings.insert(propertyName, value);
    }

    bool success = true;
    if(!sourceContent.isEmpty()) {
        // matches '$property' or '$(property, type)'
        QRegularExpressionMatchIterator matches = propertyRegex.globalMatch(sourceContent);
        QList<QString> alreadyReplaced;
        while(matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString propertyName = match.captured(1);
            // GLSL doesn't allow double underscores. Use name without underscores for identifiers.
            QString propertyNameNoUnderscores = QString(propertyName).replace("_", "");
            if(alreadyReplaced.contains(propertyName)) {
                continue;
            }
            QString targetType;
            if(match.lastCapturedIndex() > 1) {
                targetType = match.captured(2);
            }
            QRegularExpression namedRegex("\\$(\\(\\s*)?" + propertyName + "\\b(\\s*,\\s*[_a-zA-Z0-9]+\\s*\\))?");
            if(propertyName == "this") {
                // $this is a special keyword that refers to the node itself, it cannot be used for a property name
                QString thisIdentifier = identifier();
                sourceContent.replace(namedRegex, thisIdentifier);
                alreadyReplaced.append(propertyName);
                continue;
            }
            if(!mappings.contains(propertyName)) {
                // No connected property, assume internal variable that just needs a unique name
                QString propertylessIdentifier = propertyNameNoUnderscores + "_" + ShaderUtils::generateName();
                sourceContent.replace(namedRegex, propertylessIdentifier);
                alreadyReplaced.append(propertyName);
                continue;
            }
            QVariant value = mappings[propertyName];
            if(!value.isValid()) {
                continue;
            }

            if(value.canConvert(QVariant::List)) {
                int i = 0;
                QVariantList list = value.toList();
                if(list.count() < 1) {
                    continue;
                }
                for(const QVariant& listValue : list) {
                    QVariant value = listValue;
                    if(listValue.type() == QVariant::String) {
                        value = QColor(listValue.toString()); // TODO do proper conversion
                    }
                    QString targetIdentifier;
                    QString sourceType;
                    if(ShaderNode *node = qvariant_cast<ShaderNode*>(listValue)) {
                        const auto &nodeSetup = node->setup(shaderBuilder);
                        success = success && nodeSetup.m_ok;
                        m_resolvedDependencies.insert(node);
                        m_resolvedDependencies.unite(nodeSetup.m_dependencies);
                        targetIdentifier = node->identifier();
                        sourceType = node->type();
                    } else {
                        targetIdentifier = createUniform(propertyName, listValue, shaderBuilder);
                        sourceType = glslType(listValue);
                    }
                    // replaces '$property' or '$(property, type)'
                    QRegularExpression indexedRegex("\\$(\\(\\s*)?" + propertyName + "\\b\\[" + QString::number(i) +"\\](\\s*,\\s*[_a-zA-Z0-9]+\\s*\\))?");
                    sourceContent.replace(indexedRegex, ShaderUtils::convert(sourceType, targetType, targetIdentifier));

                    // Replace any singular occurences with first item
                    if(i == 0) {
                        QRegularExpression namedTargetRegex("\\$(\\(\\s*)?" + propertyName + "(?![\\w\\W\\[])(\\s*,\\s*" + targetType + "\\s*\\))?");
                        sourceContent.replace(namedTargetRegex, ShaderUtils::convert(sourceType, targetType, targetIdentifier));
                    }

                    i += 1;
                }

                alreadyReplaced.append(propertyName);
                continue;
            }

            QString typeName = value.typeName();
            if(typeName.startsWith("QQmlListProperty")) {
                QQmlListProperty<QObject> *list = static_cast<QQmlListProperty<QObject>*>(value.data());
                int count = list->count(list);
                for(int i = 0; i < count; i++) {
                    QObject *object = list->at(list, i);
                    ShaderNode *node = qobject_cast<ShaderNode*>(object);
                    if(!node) {
                        qWarning() << "ERROR: Could not convert listed object to ShaderNode:" << object;
                        return {false, QSet<ShaderNode*>()};
                    }
                    const auto& nodeSetup = node->setup(shaderBuilder);
                    success = success && nodeSetup.m_ok;
                    m_resolvedDependencies.insert(node);
                    m_resolvedDependencies.unite(nodeSetup.m_dependencies);
                    QString targetIdentifier = node->identifier();
                    QString sourceType = node->type();
                    // replaces '$property' or '$(property, type)'
                    QRegularExpression indexedRegex("\\$(\\(\\s*)?" + propertyName + "\\b\\[" + QString::number(i) +"\\](\\s*,\\s*[_a-zA-Z0-9]+\\s*\\))?");
                    sourceContent.replace(indexedRegex, ShaderUtils::convert(sourceType, targetType, targetIdentifier));
                }
                alreadyReplaced.append(propertyName);
                continue;
            }

            ShaderNode *node = qvariant_cast<ShaderNode*>(value);

            QString targetIdentifier;
            QString sourceType;
            if(node) {
                const auto& nodeSetup = node->setup(shaderBuilder);
                success = success && nodeSetup.m_ok;
                m_resolvedDependencies.insert(node);
                m_resolvedDependencies.unite(nodeSetup.m_dependencies);
                targetIdentifier = node->identifier();
                sourceType = node->type();
            } else {
                targetIdentifier = createUniform(propertyName, value, shaderBuilder);
                sourceType = glslType(value);
            }
            if(targetIdentifier.isEmpty()) {
                continue;
            }
            // replaces '$property' or '$(property, type)'
            if(targetType.isEmpty()) { // TODO do similar splits for indexed and others
                QRegularExpression namedTargetRegex("\\$(\\(\\s*)?" + propertyName + "\\b");
                sourceContent.replace(namedTargetRegex, targetIdentifier);
            } else {
                QRegularExpression namedTargetRegex("\\$(\\(\\s*)?" + propertyName + "\\b\\s*,\\s*" + targetType + "\\s*\\)");
                sourceContent.replace(namedTargetRegex, ShaderUtils::convert(sourceType, targetType, targetIdentifier));
            }

//            alreadyReplaced.append(propertyName);
        }
    }

    if(!tempIdentifier.isEmpty()) {
        m_hasSetup = true;
    }
    m_resolvedSource = sourceContent;
    return {success, m_resolvedDependencies};
}

void ShaderNode::reset() const
{
    for(ShaderNode* dependency : m_resolvedDependencies) {
        dependency->reset();
    }
    m_hasGeneratedBody = false;
    m_hasGeneratedHeader = false;
    m_hasSetup = false;
}

void ShaderNode::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    // GLSL doesn't allow double underscores in names. Remove all before generating an identifier name.
    m_identifier = QString(name).replace("_", "") + "_" + ShaderUtils::generateName();

    emit identifierChanged(m_identifier);
    emit nameChanged(name);
}

void ShaderNode::setType(QString type)
{
    if (m_type == type)
        return;

    m_type = type;

    emit typeChanged(type);
}

void ShaderNode::setResult(QString result)
{
    if (m_result == result)
        return;

    m_result = result;
    emit resultChanged(result);
}

void ShaderNode::setHeader(QString header)
{
    if (m_header == header)
        return;

    m_header = header;
    emit headerChanged(header);
}

void ShaderNode::setSource(QString source)
{
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged(source);
}

void ShaderNode::setRequirement(bool requirement)
{
    if (m_requirement == requirement)
        return;

    m_requirement = requirement;
    emit requirementChanged(requirement);
}

void ShaderNode::setHeaderFile(QUrl headerFile)
{
    if (m_headerFile == headerFile)
        return;

    QString fileName = QQmlFile::urlToLocalFileOrQrc(headerFile);
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    setHeader(file.readAll());

    m_headerFile = headerFile;
    emit headerFileChanged(headerFile);
}

void ShaderNode::setExportedTypeName(QString exportedTypeName)
{
    if (m_exportedTypeName == exportedTypeName)
        return;

    m_exportedTypeName = exportedTypeName;
    emit exportedTypeNameChanged(exportedTypeName);
}

void ShaderNode::setArrayProperties(QStringList arrayProperties)
{
    if (m_arrayProperties == arrayProperties)
        return;

    m_arrayProperties = arrayProperties;
    emit arrayPropertiesChanged(arrayProperties);
}

void ShaderNode::setHeaderFiles(QList<QUrl> headerFiles)
{
    if (m_headerFiles == headerFiles)
        return;

    QString headerData = "";
    for(auto& headerFile : headerFiles) {
        QString fileName = QQmlFile::urlToLocalFileOrQrc(headerFile);
        QFile file(fileName);
        file.open(QFile::ReadOnly);
        headerData += file.readAll() + "\n";
    }
    setHeader(headerData);

    m_headerFiles = headerFiles;
    emit headerFilesChanged(headerFiles);
}

QString ShaderNode::glslType(QVariant value) const
{
    return ShaderUtils::glslType(value);
}

QString ShaderNode::preferredType(QVariant value1, QVariant value2) const
{
    return ShaderUtils::preferredType(value1, value2);
}

QStringList ShaderNode::inputNames() const
{
    QStringList names;
    for(int i = 0; i < metaObject()->propertyCount(); i++) {
        QString name = metaObject()->property(i).name();
        if(builtinNames.contains(name)) {
            continue;
        }
        if(name.length() > 0 && name.at(0) == QChar('_')) {
            continue;
        }
        names.append(name);
    }
    return names;
}

QString ShaderNode::source() const
{
    return m_source;
}

bool ShaderNode::requirement() const
{
    return m_requirement;
}

QQmlListProperty<ShaderNode> ShaderNode::dependencies()
{
    return QQmlListProperty<ShaderNode>(this, m_declaredDependencies);
}

QUrl ShaderNode::headerFile() const
{
    return m_headerFile;
}

void ShaderNode::addMapping(QString propertyName, const QVariant &value)
{
    m_mappings.insert(propertyName, value);
}

void ShaderNode::removeMapping(QString propertyName)
{
    m_mappings.remove(propertyName);
}

void ShaderNode::addDependency(ShaderNode *dependency)
{
    m_declaredDependencies.append(dependency);
}

void ShaderNode::removeDependency(ShaderNode *dependency)
{
    m_declaredDependencies.removeAll(dependency);
}

void ShaderNode::clearDependencies()
{
    m_declaredDependencies.clear();
}

QString ShaderNode::exportedTypeName() const
{
    return m_exportedTypeName;
}

QStringList ShaderNode::arrayProperties() const
{
    return m_arrayProperties;
}

QList<QUrl> ShaderNode::headerFiles() const
{
    return m_headerFiles;
}
