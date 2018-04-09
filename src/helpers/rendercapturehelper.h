#ifndef RENDERCAPTUREHELPER_H
#define RENDERCAPTUREHELPER_H

#include <QObject>
#include <QQmlEngine>

#include <Qt3DRender/QRenderCaptureReply>

class RenderCaptureHelper : public QObject {
    Q_OBJECT
public:
    explicit RenderCaptureHelper(QObject* parent = nullptr);

    Q_INVOKABLE int particleAtPoint(Qt3DRender::QRenderCaptureReply* captureReply, const QPointF& point);

    static QObject* qmlInstance(QQmlEngine* engine, QJSEngine* scriptEngine);
signals:

public slots:
};

#endif // RENDERCAPTUREHELPER_H
