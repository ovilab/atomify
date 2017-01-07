#include "codeeditorbackend.h"

#include <QFileInfo>
#include <QDir>
#include <QUrlQuery>

CodeEditorBackend::CodeEditorBackend()
{

}

QString CodeEditorBackend::text() const
{
    return m_text;
}

QUrl CodeEditorBackend::fileUrl() const
{
    return m_fileUrl;
}

QString CodeEditorBackend::fileName() const
{
    if(m_fileUrl.toString().isEmpty()) {
        return QString("untitled");
    }
    return m_fileUrl.fileName();
}

void CodeEditorBackend::setText(QString text)
{
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged(text);
}

void CodeEditorBackend::setFileUrl(QUrl fileUrl)
{
    fileUrl = QUrl::fromLocalFile(fileUrl.toLocalFile());
//    qDebug() << "Url: ", fileUrl;
//    qDebug() << "Url path: ", fileUrl.path();
    // fileUrl = QUrl(fileUrl.path());

    if (m_fileUrl == fileUrl)
        return;

    m_fileUrl = fileUrl;
    emit fileUrlChanged(fileUrl);
    emit folderChanged(folder());
    emit fileNameChanged(fileName());
}

bool CodeEditorBackend::save()
{
    QString filePath = m_fileUrl.toLocalFile();
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open file for saving" << filePath;
        qDebug() << "ERROR:" << file.errorString();
        return false;
    }

    file.write(m_text.toUtf8());
    file.close();
    qDebug() << "Saved to file" << filePath;
    return true;
}

QString CodeEditorBackend::folder() const
{
    QFileInfo info(m_fileUrl.toLocalFile());
    return info.dir().absolutePath();
}

bool CodeEditorBackend::fileExists(QString path)
{
    QUrl url(path);
    QFileInfo info(url.toLocalFile());
    return info.exists();
}

QString CodeEditorBackend::cleanPath(QString path)
{
    return "file://"+QUrl(path).toLocalFile();
}

QVariantMap CodeEditorBackend::getParameters(QUrl path)
{
    QVariantMap map;

    QUrlQuery query(path);
    auto items = query.queryItems();
    for(auto item : items) {
        map[item.first] = item.second;
    }
    return map;
}

bool CodeEditorBackend::load()
{
    QString filePath = m_fileUrl.toLocalFile();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file " << filePath;
        return false;
    }
    QByteArray content = file.readAll();
    setText(QString::fromUtf8(content.constData(), content.length()));

    file.close();
    qDebug() << "Read contents of" << filePath;
    return true;
}
