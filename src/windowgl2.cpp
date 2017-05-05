#include "windowgl2.h"

WindowGL2::WindowGL2(QQuickWindow *parent) : QQuickWindow(parent)
{
    QSurfaceFormat format;
    setFormat(format);
}
