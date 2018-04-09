#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>

class Clipboard : public QObject {
    Q_OBJECT
public:
    explicit Clipboard(QObject* parent = nullptr);
    Q_INVOKABLE void setText(QString text);
signals:

public slots:
};

#endif // CLIPBOARD_H
