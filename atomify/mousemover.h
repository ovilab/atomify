#ifndef MOUSEMOVER_H
#define MOUSEMOVER_H

#include <QWindow>

class MouseMover : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showCursor READ showCursor WRITE setShowCursor NOTIFY showCursorChanged)
    Q_PROPERTY(QWindow* window READ window WRITE setWindow NOTIFY windowChanged)
public:
    explicit MouseMover(QObject *parent = 0);
    virtual ~MouseMover();
    bool showCursor() const;
    QWindow* window() const;

signals:
    void showCursorChanged(bool arg);
    void windowChanged(QWindow* window);

public slots:
    void move(int x, int y);
    void setShowCursor(bool arg);
    void setWindow(QWindow* window);

private:
    bool m_showCursor;
    QWindow* m_window;
};

#endif // MOUSEMOVER_H
