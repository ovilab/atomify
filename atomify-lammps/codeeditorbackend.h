#ifndef CODEEDITORBACKEND_H
#define CODEEDITORBACKEND_H

#include <QQuickItem>
#include <QVariantMap>

class CodeEditorBackend : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl WRITE setFileUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(QString folder READ folder NOTIFY folderChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
public:
    CodeEditorBackend();
    QString text() const;
    QUrl fileUrl() const;
    QString fileName() const;
    Q_INVOKABLE bool load();
    Q_INVOKABLE bool save();
    QString folder() const;
    Q_INVOKABLE bool fileExists(QString path);
    Q_INVOKABLE QVariantMap getParameters(QUrl path);
signals:
    void textChanged(QString text);
    void fileUrlChanged(QUrl fileUrl);
    void fileNameChanged(QString fileName);
    void folderChanged(QString folder);

public slots:
    void setText(QString text);
    void setFileUrl(QUrl fileUrl);

private:
    QString m_text;
    QUrl m_fileUrl;
};

#endif // CODEEDITORBACKEND_H
