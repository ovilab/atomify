#ifndef FIGURE_H
#define FIGURE_H

#include <QQuickPaintedItem>
#include <QPainter>

class Figure : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(float xMin READ xMin WRITE setXMin NOTIFY xMinChanged)
    Q_PROPERTY(float xMax READ xMax WRITE setXMax NOTIFY xMaxChanged)
    Q_PROPERTY(float yMin READ yMin WRITE setYMin NOTIFY yMinChanged)
    Q_PROPERTY(float yMax READ yMax WRITE setYMax NOTIFY yMaxChanged)
    Q_PROPERTY(QString xLabel READ xLabel WRITE setXLabel NOTIFY xLabelChanged)
    Q_PROPERTY(QString yLabel READ yLabel WRITE setYLabel NOTIFY yLabelChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
private:
    float m_xMin = 0;
    float m_xMax = 0;
    float m_yMin = 0;
    float m_yMax = 0;
    QRectF m_figureRectangle;

    int numTicksX() { return width() / 100; }
    int numTicksY() { return height() / 100; }

    void drawAxis(QPainter *painter);
    void drawTicks(QPainter *painter);
    void drawTickText(QPainter *painter);
    void drawGraphs(QPainter *painter);
    void drawLabels(QPainter *painter);
    QString m_xLabel;
    QString m_yLabel;
    QString m_title;
    QFont m_font;
    void drawText(QPointF position, QString text, QPainter *painter);
    QColor m_color = "white";

public:
    Figure(QQuickItem *parent = 0);
    virtual void paint(QPainter *painter);

    QRectF figureRectangle() { return m_figureRectangle; }
    QPointF scaled(const QPointF &p);
    QRectF scaled(const QRectF &rect);
    QLineF scaled(const QLineF &line);
    QPointF scaled(const QPointF &p, const QRectF &rect, const QPointF delta = QPointF(0,0));

    float xMin() const { return m_xMin; }
    float xMax() const { return m_xMax; }
    float yMin() const { return m_yMin; }
    float yMax() const { return m_yMax; }

    QString xLabel() const;
    QString yLabel() const;
    QString title() const;
    QFont font() const;
    QColor color() const;

public slots:
    void setXMin(float xMin);
    void setXMax(float xMax);
    void setYMin(float yMin);
    void setYMax(float yMax);
    void setXLabel(QString xLabel);
    void setYLabel(QString yLabel);
    void setTitle(QString title);
    void setFont(QFont font);
    void setColor(QColor color);

signals:
    void xMinChanged(float xMin);
    void xMaxChanged(float xMax);
    void yMinChanged(float yMin);
    void yMaxChanged(float yMax);
    void xLabelChanged(QString xLabel);
    void yLabelChanged(QString yLabel);
    void titleChanged(QString title);
    void fontChanged(QFont font);
    void colorChanged(QColor color);
};

#endif // FIGURE_H
