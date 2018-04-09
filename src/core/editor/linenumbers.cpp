#include "linenumbers.h"
#include <QDebug>
#include <QPainter>
#include <algorithm>
#include <cmath>

LineNumbers::LineNumbers(QQuickPaintedItem* parent)
    : QQuickPaintedItem(parent)
{
}

int LineNumbers::lineCount() const
{
    return m_lineCount;
}

int LineNumbers::scrollY() const
{
    return m_scrollY;
}

void LineNumbers::setLineCount(int lineCount)
{
    if (m_lineCount == lineCount)
        return;

    m_lineCount = lineCount;
    emit lineCountChanged(lineCount);
}

void LineNumbers::setScrollY(int scrollY)
{
    if (m_scrollY == scrollY)
        return;

    m_scrollY = scrollY;
    emit scrollYChanged(scrollY);
}

void LineNumbers::setLineHeight(float lineHeight)
{
    if (m_lineHeight == lineHeight)
        return;

    m_lineHeight = lineHeight;
    emit lineHeightChanged(lineHeight);
}

void LineNumbers::setText(QString text)
{
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged(text);
}

void LineNumbers::setCursorPosition(int cursorPosition)
{
    if (m_cursorPosition == cursorPosition)
        return;

    m_cursorPosition = cursorPosition;
    emit cursorPositionChanged(cursorPosition);
}

void LineNumbers::setSelectionStart(int selectionStart)
{
    if (m_selectionStart == selectionStart)
        return;

    m_selectionStart = selectionStart;
    emit selectionStartChanged(selectionStart);
}

void LineNumbers::setSelectionEnd(int selectionEnd)
{
    if (m_selectionEnd == selectionEnd)
        return;

    m_selectionEnd = selectionEnd;
    emit selectionEndChanged(selectionEnd);
}

void LineNumbers::setCurrentLine(int currentLine)
{
    if (m_currentLine == currentLine)
        return;

    m_currentLine = currentLine;
    emit currentLineChanged(currentLine);
}

void LineNumbers::setErrorLine(int errorLine)
{
    if (m_errorLine == errorLine)
        return;

    m_errorLine = errorLine;
    emit errorLineChanged(errorLine);
}

void LineNumbers::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
}

void LineNumbers::setSelectedColor(QColor selectedColor)
{
    if (m_selectedColor == selectedColor)
        return;

    m_selectedColor = selectedColor;
    emit selectedColorChanged(selectedColor);
}

void LineNumbers::setErrorColor(QColor errorColor)
{
    if (m_errorColor == errorColor)
        return;

    m_errorColor = errorColor;
    emit errorColorChanged(errorColor);
}

void LineNumbers::setActiveColor(QColor activeColor)
{
    if (m_activeColor == activeColor)
        return;

    m_activeColor = activeColor;
    emit activeColorChanged(activeColor);
}

void LineNumbers::setFont(QFont font)
{
    if (m_font == font)
        return;

    m_font = font;
    emit fontChanged(font);
}

void LineNumbers::paint(QPainter* painter)
{
    // Find current line
    QString untilSelectedText = m_text.mid(0, selectionStart());
    int selectedTextStartLine = untilSelectedText.count(QRegExp("[\r\n]")) + 1;

    QString selectedText = m_text.mid(selectionStart(), (selectionEnd() - selectionStart()));
    int numLinesSelected = selectedText.count(QRegExp("[\r\n]")) + 1;

    QString textUntilCursorPosition = m_text.mid(0, m_cursorPosition);
    int cursorLine = textUntilCursorPosition.count(QRegExp("[\r\n]")) + 1;

    int firstLineVisible = m_scrollY / m_lineHeight;
    int lineHeight = int(round(m_lineHeight));
    int rest = (m_scrollY > 0) ? (m_scrollY % lineHeight) : 0;
    // The last visible line is either the last line in the textfield or if we have scrolled as far as we get with current size
    int lastLineVisible = std::min(firstLineVisible + int(height() / m_lineHeight) + 1, m_lineCount);
    int numLines = lastLineVisible - firstLineVisible;
    for (int i = 0; i < numLines; i++) {
        int lineNumber = i + firstLineVisible + 1;
        QString text = QString("%1").arg(lineNumber);
        float y = i * m_lineHeight - rest;
        QRectF textRect(0, y, width(), m_lineHeight);

        painter->setFont(m_font);
        painter->setPen(m_color);
        if (lineNumber >= selectedTextStartLine && lineNumber < selectedTextStartLine + numLinesSelected) {
            painter->setPen(m_selectedColor);
        }
        if (lineNumber == cursorLine) {
            painter->setPen(m_selectedColor);
        }
        if (lineNumber == m_currentLine) {
            painter->setPen(m_activeColor);
        }
        if (lineNumber == m_errorLine) {
            painter->setPen(m_errorColor);
        }
        QTextOption options;
        options.setAlignment(Qt::AlignRight);
        painter->drawText(textRect, text, options);
    }
}

QString LineNumbers::text() const
{
    return m_text;
}

int LineNumbers::cursorPosition() const
{
    return m_cursorPosition;
}

int LineNumbers::selectionStart() const
{
    return m_selectionStart;
}

int LineNumbers::selectionEnd() const
{
    return m_selectionEnd;
}

int LineNumbers::currentLine() const
{
    return m_currentLine;
}

int LineNumbers::errorLine() const
{
    return m_errorLine;
}

QColor LineNumbers::color() const
{
    return m_color;
}

QColor LineNumbers::selectedColor() const
{
    return m_selectedColor;
}

QColor LineNumbers::errorColor() const
{
    return m_errorColor;
}

QColor LineNumbers::activeColor() const
{
    return m_activeColor;
}

QFont LineNumbers::font() const
{
    return m_font;
}

float LineNumbers::lineHeight() const
{
    return m_lineHeight;
}
