#ifndef LINENUMBERS_H
#define LINENUMBERS_H

#include <QQuickPaintedItem>

class LineNumbers : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int lineCount READ lineCount WRITE setLineCount NOTIFY lineCountChanged)
    Q_PROPERTY(int scrollY READ scrollY WRITE setScrollY NOTIFY scrollYChanged)
    Q_PROPERTY(float lineHeight READ lineHeight WRITE setLineHeight NOTIFY lineHeightChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

public:
    explicit LineNumbers(QQuickPaintedItem *parent = nullptr);
    int lineCount() const;
    int scrollY() const;
    float lineHeight() const;
    virtual void paint(QPainter *painter) override;
    QString text() const;
    int cursorPosition() const;

    int selectionStart() const;

    int selectionEnd() const;

signals:
    void lineCountChanged(int lineCount);
    void scrollYChanged(int scrollY);
    void lineHeightChanged(float lineHeight);
    void textChanged(QString text);
    void cursorPositionChanged(int cursorPosition);

    void selectionStartChanged(int selectionStart);

    void selectionEndChanged(int selectionEnd);

public slots:
    void setLineCount(int lineCount);
    void setScrollY(int scrollY);
    void setLineHeight(float lineHeight);
    void setText(QString text);
    void setCursorPosition(int cursorPosition);

    void setSelectionStart(int selectionStart);

    void setSelectionEnd(int selectionEnd);

private:
    int m_lineCount = 0;
    int m_scrollY = 0;
    float m_lineHeight = 0;
    int m_cursorPosition = 0;
    QString m_text;
    int m_selectionStart = 0;
    int m_selectionEnd = 0;
};

#endif // LINENUMBERS_H
