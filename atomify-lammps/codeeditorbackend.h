#ifndef CODEEDITORBACKEND_H
#define CODEEDITORBACKEND_H

#include <QQuickItem>

class CodeEditorBackend : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl WRITE setFileUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
public:
    CodeEditorBackend();
    QString text() const;
    QUrl fileUrl() const;
    QString fileName() const;
    Q_INVOKABLE bool load();
    Q_INVOKABLE bool save();
signals:
    void textChanged(QString text);
    void fileUrlChanged(QUrl fileUrl);
    void fileNameChanged(QString fileName);

public slots:
    void setText(QString text);
    void setFileUrl(QUrl fileUrl);
    void setFileName(QString fileName);
private:
    QString m_text;
    QUrl m_fileUrl;
    QString m_fileName;

};

#endif // CODEEDITORBACKEND_H
