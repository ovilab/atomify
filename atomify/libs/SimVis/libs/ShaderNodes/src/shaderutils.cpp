#include "shaderutils.h"

#include "shadernode.h"
#include "shaderbuilderbinding.h"
#include "shaderparameter.h"

#include <QDebug>
#include <QMetaProperty>
#include <QJSEngine>
#include <QQmlExpression>
#include <QQmlEngine>
#include <QQmlFile>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QFileInfo>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>

using Qt3DRender::QTexture2D;
using Qt3DRender::QParameter;

int ShaderUtils::m_nameCounter = 0;
QMutex ShaderUtils::m_nameMutex;

ShaderUtils::ShaderUtils()
{
}

bool ShaderUtils::isList(const QVariant &value)
{
    return value.canConvert(QVariant::List);
}

int ShaderUtils::componentCount(const QVariant &value)
{
    QString type = glslType(value);
    if(type == "vec2") {
        return 2;
    }
    if(type == "vec3") {
        return 3;
    }
    if(type == "vec4") {
        return 4;
    }
    return 1;
}

QString ShaderUtils::serialize(const QVariant &value)
{
    switch(value.type()) {
    case QVariant::Bool:
        return value.toString();
    case QVariant::Int:
        return QString::number(value.toInt());
    case QVariant::Double:
        return QString::number(value.toDouble(), 'g', 3);
    case QVariant::Vector2D: {
        QVector2D vec2 = qvariant_cast<QVector2D>(value);
        return QString("Qt.vector2d(" +
                       QString::number(vec2.x()) + ", " +
                       QString::number(vec2.y()) + ")");
    }
    case QVariant::Vector3D: {
        QVector3D vec3 = qvariant_cast<QVector3D>(value);
        return QString("Qt.vector3d(" +
                       QString::number(vec3.x()) + ", " +
                       QString::number(vec3.y()) + ", " +
                       QString::number(vec3.z()) + ")");
    }
    case QVariant::Vector4D: {
        QVector4D vec4 = qvariant_cast<QVector4D>(value);
        return QString("Qt.vector4d(" +
                       QString::number(vec4.x()) + ", " +
                       QString::number(vec4.y()) + ", " +
                       QString::number(vec4.z()) + ", " +
                       QString::number(vec4.w()) + ")");
    }
    case QVariant::Color:{
        QColor color = qvariant_cast<QColor>(value);
        return QString("Qt.rgba(" +
                       QString::number(color.redF()) + ", " +
                       QString::number(color.greenF()) + ", " +
                       QString::number(color.blueF()) + ", " +
                       QString::number(color.alphaF()) + ")");
    }
    case QVariant::String:
        return "\"" + value.toString() + "\"";
    default:
        return QString("");
        break;
    }
}

ShaderUtils::Type ShaderUtils::variantType(const QVariant &value)
{
    return static_cast<ShaderUtils::Type>(value.type());
}

bool ShaderUtils::isShaderNode(const QVariant &value)
{
    ShaderNode* node = qvariant_cast<ShaderNode*>(value);
    if(node) {
        return true;
    } else {
        return false;
    }
}

bool ShaderUtils::isShaderBuilderBinding(const QVariant &value)
{
    ShaderBuilderBinding* builderBinding = qvariant_cast<ShaderBuilderBinding*>(value);
    if(builderBinding) {
        return true;
    } else {
        return false;
    }
}

QString ShaderUtils::glslType(const QVariant &value)
{
    ShaderNode *node = qvariant_cast<ShaderNode*>(value);
    if(node) {
        return node->type();
    }
    QTexture2D *texture = qvariant_cast<QTexture2D*>(value);
    if(texture) {
        return "sampler2D";
    }
//    ShaderParameter *parameter = qvariant_cast<ShaderParameter*>(value);
//    if(parameter) {
//        return glslType(parameter->value());
//    }

    QString typeName = value.typeName();
    if(typeName == "QQmlListReference") {
        return "_invalid_aka_qml_list_";
    }
    if(value.canConvert(QVariant::List)) {
        int highestComponentCount = 0;
        QString bestType = "float";
        QVariantList list = qvariant_cast<QVariantList>(value);
        for(QVariant listValue : list) {
            int currentComponentCount = componentCount(listValue);
            if(currentComponentCount > highestComponentCount) {
                highestComponentCount = currentComponentCount;
                bestType = glslType(listValue);
            }
        }
        return bestType;
    }
    switch(value.type()) {
    case QVariant::Bool:
        return QString("bool");
    case QVariant::Int:
        return QString("float");
    case QVariant::Double:
        return QString("float");
    case QVariant::Vector2D:
        return QString("vec2");
    case QVariant::Vector3D:
        return QString("vec3");
    case QVariant::Vector4D:
        return QString("vec4");
    case QVariant::Color:
        return QString("vec4");
    case QVariant::String:
        // assume strings to be colors because there are no strings in GLSL
        return QString("vec4");
    default:
        qWarning() << "ShaderUtils::glslType(): Could not identify type of" << value.typeName() << value;
        return QString("invalid");
        break;
    }
}

QColor ShaderUtils::hsv(double h, double s, double v, double a)
{
    return QColor::fromHsvF(h, s, v, a);
}

double ShaderUtils::hsvHue(const QColor &color)
{
    return color.hsvHueF();
}

double ShaderUtils::hsvSaturation(const QColor &color)
{
    return color.hsvSaturationF();
}

double ShaderUtils::hsvValue(const QColor &color)
{
    return color.valueF();
}

bool ShaderUtils::fileExists(const QUrl &fileUrl)
{
    QString fileName = QQmlFile::urlToLocalFileOrQrc(fileUrl);
    QFileInfo info(fileName);
    if(info.exists() && info.isFile()) {
        return true;
    } else {
        return false;
    }
}

QString ShaderUtils::preferredType(const QVariant &value1, const QVariant &value2)
{
    QStringList preferenceOrder = {
        "bool",
        "float",
        "vec2",
        "vec3",
        "vec4"
    };

    QString type1 = glslType(value1);
    QString type2 = glslType(value2);

    int value1Preference = preferenceOrder.indexOf(type1);
    int value2Preference = preferenceOrder.indexOf(type2);

    if(value1Preference > value2Preference) {
        return type1;
    } else {
        return type2;
    }

}

QObject *ShaderUtils::qmlInstance(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);
    return new ShaderUtils;
}

QString ShaderUtils::convert(const QString &sourceType, const QString &targetType, const QString &identifier)
{
    const QString &v = identifier;
    if(targetType.isEmpty() || sourceType == targetType) {
        return v;
    }

    QVariantMap scalar{
        {"bool", "bool(" + v + ")"},
        {"int", "int(" + v + ")"},
        {"uint", "uint(" + v + ")"},
        {"float", "float(" + v + ")"},
        {"double", "double(" + v + ")"},
        {"vec2", "vec2(" + v + ", " + v + ")"},
        {"vec3", "vec3(" + v + ", " + v + ", " + v + ")"},
        {"vec4", "vec4(" + v + ", " + v + ", " + v + ", 1.0)"}
    };

    QVariantMap empty{
        {"bool", "false"},
        {"int", "0"},
        {"uint", "0"},
        {"float", "0.0"},
        {"double", "0.0"},
        {"vec3", "vec2(0.0, 0.0)"},
        {"vec3", "vec3(0.0, 0.0, 0.0)"},
        {"vec4", "vec4(0.0, 0.0, 0.0, 1.0)"}
    };

    // conversions from->to->implementation
    QVariantMap conversions{
        {"bool", scalar},
        {"int", scalar},
        {"uint", scalar},
        {"float", scalar},
        {"double", scalar},
        {"vec2", QVariantMap{
                {"float", "0.5 * (" + v + ".x + " + v + ".y)"},
                {"vec3", "vec3(" + v + ", 0.0)"},
                {"vec4", "vec4(" + v + ", 0.0, 1.0)"}
            }
        },
        {"vec3", QVariantMap{
                {"float", "1.0 / 3.0 * (" + v + ".x + " + v + ".y + " + v + ".z)"},
                {"vec2", v + ".xy"},
                {"vec4", "vec4(" + v + ", 1.0)"}
            }
        },
        {"vec4", QVariantMap{
                {"float", "0.25 * (" + v + ".x + " + v + ".y + " + v + ".z + " + v + ".w)"},
                {"vec2", v + ".xy"},
                {"vec3", v + ".xyz"}
            }
        },
        {"sampler2D", QVariantMap {
                {"float", "texture(" + v + ", vec2(0.0, 0.0)).r"},
                {"vec2", "texture(" + v + ", vec2(0.0, 0.0)).rg"},
                {"vec3", "texture(" + v + ", vec2(0.0, 0.0)).rgb"},
                {"vec4", "texture(" + v + ", vec2(0.0, 0.0)).rgba"}
            }
        }
    };
    if(conversions.contains(sourceType)) {
        QVariantMap typeConversions = conversions[sourceType].toMap();
        if(typeConversions.contains(targetType)) {
            return "(" + typeConversions[targetType].toString() + ")";
        }
    }
    qWarning() << "WARNING: ShaderUtils::convert(): No known conversion from "
               << sourceType << " to " << targetType << ".";

    if(empty.contains(targetType)) {
         qDebug() << "Will return empty target value.";
        return empty[targetType].toString();
    } else {
        qDebug() << "No empty type found for" << targetType << ". Will return targetType name and hope that it works.";
        return targetType + "()";
    }
}

QString ShaderUtils::generateName()
{
    m_nameMutex.lock();
    QString name = QString::number(m_nameCounter);
    m_nameCounter += 1;
    m_nameMutex.unlock();
    return name;
}

QString ShaderUtils::precisionQualifier(QString type)
{
    Q_UNUSED(type);
    return "highp";
}
