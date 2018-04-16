#ifndef CORE_EDITOR_CODEEDITORBACKEND_H
#define CORE_EDITOR_CODEEDITORBACKEND_H

#include <QQuickItem>
#include <QVariantMap>

class CodeEditorBackend : public QQuickItem {
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
    Q_INVOKABLE bool anyChangesOnDisk();

    QString folder() const;
    Q_INVOKABLE bool fileExists(QString path);
    Q_INVOKABLE bool filePathIsWritable(QString filePath);
    Q_INVOKABLE QString cleanPath(QString path);
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
    QString m_lastCheckedFileContents;
    QUrl m_fileUrl;
    QString readFile(bool* ok);
};

#endif // CORE_EDITOR_CODEEDITORBACKEND_H
