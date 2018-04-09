#include "rendercapturehelper.h"

#include <QtMath>

RenderCaptureHelper::RenderCaptureHelper(QObject* parent)
    : QObject(parent)
{
}

int RenderCaptureHelper::particleAtPoint(Qt3DRender::QRenderCaptureReply* captureReply, const QPointF& point)
{
    qDebug() << "Image" << captureReply->image();
    const QColor color = captureReply->image().pixelColor(point.toPoint());
    qDebug() << "Color" << color << color.redF() << color.greenF() << color.blueF() << color.red() << color.green() << color.blue();
    float number = 255.0 * (color.redF() * 255 * 255 + color.greenF() * 255 + color.blueF());
    qDebug() << "Number" << number;
    float number2 = color.red() * 255 * 255 + color.green() * 255 + color.blue();
    qDebug() << "Numbera" << number2;
    return number;
}

QObject* RenderCaptureHelper::qmlInstance(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new RenderCaptureHelper;
}
