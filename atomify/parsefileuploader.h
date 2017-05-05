#ifndef PARSEFILEUPLOADER_H
#define PARSEFILEUPLOADER_H

#include <QObject>
#include <QJSValue>
#include <QNetworkReply>

class UploadResult : public QObject
{
    Q_OBJECT
public:
    UploadResult(QObject *parent = 0);
    QJSValue callback;

public slots:
    void process(QNetworkReply* reply);
};

class QNetworkReply;

class ParseFileUploader : public QObject
{
    Q_OBJECT
public:
    explicit ParseFileUploader(QObject *parent = 0);

signals:

public slots:
    bool uploadGrabbedImage(QObject* image, const QJSValue &callback);
};

#endif // PARSEFILEUPLOADER_H
