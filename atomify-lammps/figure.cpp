#include "figure.h"
#include "graph.h"
#include <cmath>

Figure::Figure(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    connect(this, SIGNAL(xMinChanged(float)), this, SLOT(update()));
    connect(this, SIGNAL(xMaxChanged(float)), this, SLOT(update()));
    connect(this, SIGNAL(yMinChanged(float)), this, SLOT(update()));
    connect(this, SIGNAL(yMaxChanged(float)), this, SLOT(update()));
    connect(this, SIGNAL(xLabelChanged(QString)), this, SLOT(update()));
    connect(this, SIGNAL(yLabelChanged(QString)), this, SLOT(update()));
    connect(this, SIGNAL(titleChanged(QString)), this, SLOT(update()));
    m_font = QFont("times", 16);
}

void Figure::paint(QPainter *painter)
{
    // Calculate how much space we need for titles etc
    float yLabelSpace = 80;
    float titleSpace = 45;
    float xLabelSpace = 60;
    float rightSymmetrySpace = 30;
    m_figureRectangle = QRectF(QPointF(yLabelSpace, titleSpace), QPointF(width() - rightSymmetrySpace, height() - xLabelSpace));
    // m_figureRectangle = scaled(m_figureRectangleNormalized);
    QBrush brush(m_color);
    painter->setBackground(brush);
    painter->setBrush(brush);
    painter->drawRect(QRect(0,0,width(), height()));
    drawAxis(painter);
    drawTicks(painter);
    drawTickText(painter);
    drawLabels(painter);
    drawGraphs(painter);
    painter->setRenderHint(QPainter::Antialiasing);
}

void Figure::drawAxis(QPainter *painter) {
    QVector<QLineF> lines;
    lines.push_back(QLineF(m_figureRectangle.bottomLeft(),m_figureRectangle.bottomRight()));
    lines.push_back(QLineF(m_figureRectangle.bottomRight(),m_figureRectangle.topRight()));
    lines.push_back(QLineF(m_figureRectangle.topRight(),m_figureRectangle.topLeft()));
    lines.push_back(QLineF(m_figureRectangle.topLeft(),m_figureRectangle.bottomLeft()));
    painter->drawLines(lines);
}

void Figure::drawTicks(QPainter *painter) {
    float deltaX = m_figureRectangle.width() / (numTicksX()+1);
    float deltaY = m_figureRectangle.height() / (numTicksY()+1);
    float tickLengthHalf = 5;
    QVector<QLineF> lines;
    for(int i=1; i<=numTicksX(); i++) {
        float x = m_figureRectangle.left() + i*deltaX;
        float y1 = m_figureRectangle.bottom();
        float y2 = m_figureRectangle.top();

        QPointF p1(x, y1);
        QPointF p2(x, y1-tickLengthHalf);
        lines.push_back(QLineF(p1, p2));
        p1.setY(y2+tickLengthHalf);
        p2.setY(y2);
        lines.push_back(QLineF(p1, p2));
    }

    for(int i=1; i<=numTicksY(); i++) {
        float x1 = m_figureRectangle.left();
        float x2 = m_figureRectangle.right();
        float y = m_figureRectangle.top() + i*deltaY;
        QPointF p1(x1+tickLengthHalf, y);
        QPointF p2(x1, y);
        lines.push_back(QLineF(p1, p2));

        p1.setX(x2);
        p2.setX(x2-tickLengthHalf);
        lines.push_back(QLineF(p1, p2));
    }
    painter->drawLines(lines);
}

void Figure::drawTickText(QPainter *painter) {
    float deltaX = m_figureRectangle.width() / (numTicksX()+1);
    float deltaY = m_figureRectangle.height() / (numTicksY()+1);
    float deltaXValue = (xMax()-xMin()) / (numTicksX()+1);
    float deltaYValue = (yMax()-yMin()) / (numTicksY()+1);
    painter->setFont(m_font);
    QFontMetrics fm(m_font);
    for(int i=0; i<numTicksX()+2; i++) {
        float xValue = xMin() + i*deltaXValue;
        QString text = QString::number(xValue, 'f', 2);
        int textWidthHalf = 0.5*fm.width(text);
        int textHeightHalf = 0.5*fm.height();

        float x = m_figureRectangle.left() + i*deltaX;
        float y = m_figureRectangle.bottom() + 2.0*textHeightHalf;

        QRectF textBox = QRectF(QPointF(x-textWidthHalf, y-textHeightHalf), QPointF(x+textWidthHalf, y+textHeightHalf));
        painter->drawText(textBox, text);
    }

    for(int i=0; i<numTicksY()+2; i++) {
        float yValue = yMax() - i*deltaYValue;
        QString text = QString::number(yValue, 'f', 2);
        int textWidthHalf = 0.5*fm.width(text);
        int textHeightHalf = 0.5*fm.height();

        float x = m_figureRectangle.left() - 1.8*textWidthHalf;
        float y = m_figureRectangle.top() + i*deltaY;

        QRectF textBox = QRectF(QPointF(x-textWidthHalf, y-textHeightHalf), QPointF(x+textWidthHalf, y+textHeightHalf));
        painter->drawText(textBox, text);
    }
}

void Figure::drawGraphs(QPainter *painter) {
    QList<Graph*> graphs = findChildren<Graph*>();
    for(Graph *graph : graphs) {
        graph->paint(this, painter);
    }
}

void Figure::drawText(QPointF position, QString text, QPainter *painter) {
    QFontMetrics fm(m_font);
    float textWidthHalf = 0.5*fm.width(text);
    float textHeightHalf = 0.5*fm.height();
    QRectF textBox(QPointF(position.x()-textWidthHalf, position.y()-textHeightHalf), QPointF(position.x()+textWidthHalf, position.y()+textHeightHalf));
    painter->drawText(textBox, text);
}

void Figure::drawLabels(QPainter *painter)
{
    painter->save();
    // X label
    float x = m_figureRectangle.left() + 0.5*m_figureRectangle.width();
    float y = m_figureRectangle.bottom()+40;
    drawText(QPointF(x,y), m_xLabel, painter);

    // Y label
    y = m_figureRectangle.left() - 60;
    x = -m_figureRectangle.top()-0.5*m_figureRectangle.height();
    painter->rotate(-90);
    drawText(QPointF(x,y), m_yLabel, painter);
    painter->rotate(90);

    // Title
    m_font.setBold(true);
    painter->setFont(m_font);
    x = m_figureRectangle.left() + 0.5*m_figureRectangle.width();
    y = m_figureRectangle.top() - 25;
    drawText(QPointF(x,y), m_title, painter);
    m_font.setBold(false);
    painter->restore();
}

QLineF Figure::scaled(const QLineF &line) {
    return QLineF(scaled(line.p1()), scaled(line.p2()));
}

QPointF Figure::scaled(const QPointF &p) {
    return QPointF(p.x()*width(), p.y()*height());
}

QPointF Figure::scaled(const QPointF &p, const QRectF &rect, const QPointF delta) {
    return QPointF(p.x()*rect.width() + delta.x(), p.y()*rect.height() + delta.y());
}

QString Figure::xLabel() const
{
    return m_xLabel;
}

QString Figure::yLabel() const
{
    return m_yLabel;
}

QString Figure::title() const
{
    return m_title;
}

QFont Figure::font() const
{
    return m_font;
}

QColor Figure::color() const
{
    return m_color;
}

QRectF Figure::scaled(const QRectF &rect) {
    return QRectF(scaled(rect.topLeft()), scaled(rect.bottomRight()));
}

void Figure::setXMin(float xMin)
{
    if (m_xMin == xMin)
        return;

    m_xMin = xMin;
    emit xMinChanged(xMin);
}

void Figure::setXMax(float xMax)
{
    if (m_xMax == xMax)
        return;

    m_xMax = xMax;
    emit xMaxChanged(xMax);
}

void Figure::setYMin(float yMin)
{
    if (m_yMin == yMin)
        return;

    m_yMin = yMin;
    emit yMinChanged(yMin);
}

void Figure::setYMax(float yMax)
{
    if (m_yMax == yMax)
        return;

    m_yMax = yMax;
    emit yMaxChanged(yMax);
}

void Figure::setXLabel(QString xLabel)
{
    if (m_xLabel == xLabel)
        return;

    m_xLabel = xLabel;
    emit xLabelChanged(xLabel);
}

void Figure::setYLabel(QString yLabel)
{
    if (m_yLabel == yLabel)
        return;

    m_yLabel = yLabel;
    emit yLabelChanged(yLabel);
}

void Figure::setTitle(QString title)
{
    if (m_title == title)
        return;

    m_title = title;
    emit titleChanged(title);
}

void Figure::setFont(QFont font)
{
    if (m_font == font)
        return;

    m_font = font;
    emit fontChanged(font);
}

void Figure::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
}
