#include "clipboard.h"

#include <QClipboard>
#include <QGuiApplication>

Clipboard::Clipboard(QObject* parent)
    : QObject(parent)
{
}

void Clipboard::setText(QString text)
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}
