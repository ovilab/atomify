#ifndef SHADERUTILS_H
#define SHADERUTILS_H

#include <QString>
#include <QVariant>
#include <QColor>
#include <QMutex>
#include <QUrl>

class QQmlEngine;
class QJSEngine;

class ShaderUtils : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ShaderUtils)
    Q_ENUMS(Type)
public:
    ShaderUtils();

    enum Type {
        Invalid = QMetaType::UnknownType,
        Bool = QMetaType::Bool,
        Int = QMetaType::Int,
        UInt = QMetaType::UInt,
        LongLong = QMetaType::LongLong,
        ULongLong = QMetaType::ULongLong,
        Double = QMetaType::Double,
        Char = QMetaType::QChar,
        Map = QMetaType::QVariantMap,
        List = QMetaType::QVariantList,
        String = QMetaType::QString,
        StringList = QMetaType::QStringList,
        ByteArray = QMetaType::QByteArray,
        BitArray = QMetaType::QBitArray,
        Date = QMetaType::QDate,
        Time = QMetaType::QTime,
        DateTime = QMetaType::QDateTime,
        Url = QMetaType::QUrl,
        Locale = QMetaType::QLocale,
        Rect = QMetaType::QRect,
        RectF = QMetaType::QRectF,
        Size = QMetaType::QSize,
        SizeF = QMetaType::QSizeF,
        Line = QMetaType::QLine,
        LineF = QMetaType::QLineF,
        Point = QMetaType::QPoint,
        PointF = QMetaType::QPointF,
        RegExp = QMetaType::QRegExp,
        RegularExpression = QMetaType::QRegularExpression,
        Hash = QMetaType::QVariantHash,
        EasingCurve = QMetaType::QEasingCurve,
        Uuid = QMetaType::QUuid,
        ModelIndex = QMetaType::QModelIndex,
        PersistentModelIndex = QMetaType::QPersistentModelIndex,
        LastCoreType = QMetaType::LastCoreType,

        Font = QMetaType::QFont,
        Pixmap = QMetaType::QPixmap,
        Brush = QMetaType::QBrush,
        Color = QMetaType::QColor,
        Palette = QMetaType::QPalette,
        Image = QMetaType::QImage,
        Polygon = QMetaType::QPolygon,
        Region = QMetaType::QRegion,
        Bitmap = QMetaType::QBitmap,
        Cursor = QMetaType::QCursor,
        KeySequence = QMetaType::QKeySequence,
        Pen = QMetaType::QPen,
        TextLength = QMetaType::QTextLength,
        TextFormat = QMetaType::QTextFormat,
        Matrix = QMetaType::QMatrix,
        Transform = QMetaType::QTransform,
        Matrix4x4 = QMetaType::QMatrix4x4,
        Vector2D = QMetaType::QVector2D,
        Vector3D = QMetaType::QVector3D,
        Vector4D = QMetaType::QVector4D,
        Quaternion = QMetaType::QQuaternion,
        PolygonF = QMetaType::QPolygonF,
        Icon = QMetaType::QIcon,
        LastGuiType = QMetaType::LastGuiType,

        SizePolicy = QMetaType::QSizePolicy,

        UserType = QMetaType::User,
        LastType = 0xffffffff // need this so that gcc >= 3.4 allocates 32 bits for Type
    };

    Q_INVOKABLE static QString glslType(const QVariant &value);
    Q_INVOKABLE static QString convert(const QString &sourceType, const QString &targetType, const QString &identifier);
    Q_INVOKABLE static QString generateName();
    Q_INVOKABLE static QString precisionQualifier(QString type);
    Q_INVOKABLE static QString preferredType(const QVariant &value1, const QVariant &value2);
    Q_INVOKABLE static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *jsEngine);
    Q_INVOKABLE static int componentCount(const QVariant &value);
    Q_INVOKABLE static bool isList(const QVariant &value);
    Q_INVOKABLE static QString serialize(const QVariant &value);
    Q_INVOKABLE static Type variantType(const QVariant &value);
    Q_INVOKABLE static QColor hsv(double h, double s, double v, double a = 1.0);
    Q_INVOKABLE static double hsvHue(const QColor &color);
    Q_INVOKABLE static double hsvSaturation(const QColor &color);
    Q_INVOKABLE static double hsvValue(const QColor &color);
    Q_INVOKABLE static bool fileExists(const QUrl &fileName);
    Q_INVOKABLE static bool isShaderNode(const QVariant &value);
    Q_INVOKABLE static bool isShaderBuilderBinding(const QVariant &value);
private:
    static QMutex m_nameMutex;
    static int m_nameCounter;
};

#endif // SHADERUTILS_H
