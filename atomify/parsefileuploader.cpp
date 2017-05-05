#include "parsefileuploader.h"

#include <QtCore/QBuffer>
#include <QQuickItemGrabResult>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QQmlEngine>
#include <QJsonDocument>
#include <QJsonObject>

ParseFileUploader::ParseFileUploader(QObject *parent) : QObject(parent)
{
}

bool ParseFileUploader::uploadGrabbedImage(QObject *object, const QJSValue &callback)
{
    QQmlEngine *engine = qmlEngine(this);
    if (!engine) {
        qWarning("ParseFileUploader::uploadGrabbedImage: no QML Engine");
        return false;
    }

    if (!callback.isCallable()) {
        qWarning("ParseFileUploader::uploadGrabbedImage: 'callback' is not a function");
        return false;
    }

    QQuickItemGrabResult *result = qobject_cast<QQuickItemGrabResult*>(object);
    QImage image(result->image());
    QByteArray data;
    QBuffer buffer(&data);
    image.save(&buffer, "PNG");

    QUrl mResultsURL = QUrl("http://146.185.171.238:1338/parse/files/screenshot.png");
    QNetworkAccessManager* mNetworkManager = new QNetworkAccessManager(this);

    QNetworkRequest request(mResultsURL);
    request.setRawHeader("X-Parse-Application-Id", "atomify");
    request.setRawHeader("Content-Type", "image/png");

    UploadResult *uploadResult = new UploadResult(this);

    uploadResult->callback = callback;
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), uploadResult, SLOT(process(QNetworkReply*)));
    mNetworkManager->post(request, data);
    return true;
}

UploadResult::UploadResult(QObject *parent)
    : QObject(parent)
{

}

void UploadResult::process(QNetworkReply *reply) {
    QJSValueList callbackData = {QJSValue(QString(reply->readAll()))};
    callback.call(callbackData);
}
