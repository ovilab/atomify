#include "rendercapturehelper.h"

#include <QtMath>

RenderCaptureHelper::RenderCaptureHelper(QObject *parent) : QObject(parent)
{

}

int RenderCaptureHelper::particleAtPoint(Qt3DRender::QRenderCaptureReply *captureReply)
{
    const QColor color = captureReply->image().pixelColor(QPoint(0,0));
    int number = static_cast<int>(255.0 * (color.redF() * 255 * 255 + color.greenF() * 255 + color.blueF()));
    // float number2 = color.red() * 255 * 255 + color.green() * 255 + color.blue();
    return number;
}

QObject* RenderCaptureHelper::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new RenderCaptureHelper;
}
