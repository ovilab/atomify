#ifndef QMLPREVIEWER_H
#define QMLPREVIEWER_H

#include <QFileSystemWatcher>
#include <QObject>
#include <QQuickItem>
#include <QQuickView>

class QmlPreviewer : public QObject
{
    Q_OBJECT
public:
    explicit QmlPreviewer(QApplication &app);

signals:

public slots:
    void reload();
    void show();
    void setQrcPaths(QVariant qrcPaths);
private:
    QFileSystemWatcher m_watcher;
    QQuickView m_view;
    QQuickItem *m_rootItem = nullptr;
    QString m_binPath;
    QVariantList m_qrcPaths;
    QObject *m_object = nullptr;
    QString m_prefix = "/qtqmlpreview";
};

#endif // QMLPREVIEWER_H
