#include "linegraph.h"
#include "figure.h"

LineGraph::LineGraph()
{
    setDataSource(&m_defaultDataSource);
}

LineGraph::~LineGraph()
{

}

bool LineGraph::isInValueRange(const QPointF &point, const float &xMin, const float &xMax, const float &yMin, const float &yMax) {
    return point.x()>=xMin && point.x() <= xMax && point.y() >= yMin && point.y() <= yMax;
}

void LineGraph::paint(Figure *figure, QPainter *painter)
{
    painter->save();
    QPen pen(m_style);
    pen.setColor(m_color);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(m_width);
    painter->setPen(pen);

    QVector<QPointF> scaledPoints;
    float rangeX = figure->xMax()-figure->xMin();
    float rangeY = figure->yMax()-figure->yMin();
    float xMin = figure->xMin();
    float yMin = figure->yMin();
    float xMax = figure->xMax();
    float yMax = figure->yMax();

    for(int i=0; i< m_dataSource->size()-1; i++) {
        const QPointF &p1 = m_dataSource->get(i);
        const QPointF &p2 = m_dataSource->get(i+1);
        if(!isInValueRange(p1, xMin, xMax, yMin, yMax) || !isInValueRange(p2, xMin, xMax, yMin, yMax)) {
            continue;
        }

        float x1 = (p1.x()-xMin)/rangeX;
        float x2 = (p2.x()-xMin)/rangeX;

        float y1 = 1.0 - (p1.y()-yMin)/rangeY; // y is flipped
        float y2 = 1.0 - (p2.y()-yMin)/rangeY;
        scaledPoints.push_back(figure->scaled(QPointF(x1,y1), figure->figureRectangle(), figure->figureRectangle().topLeft()));
        scaledPoints.push_back(figure->scaled(QPointF(x2,y2), figure->figureRectangle(), figure->figureRectangle().topLeft()));
    }

    painter->drawLines(scaledPoints);
    scaledPoints.clear();
    setDirty(false);
    painter->restore();
}

LineGraphDataSource *LineGraph::dataSource() const
{
    return m_dataSource;
}

QColor LineGraph::color() const
{
    return m_color;
}

Qt::PenStyle LineGraph::style() const
{
    return m_style;
}

int LineGraph::width() const
{
    return m_width;
}

void LineGraphDataSource::cleanupMemory() {
    m_points.erase(m_points.begin(), m_points.begin()+m_firstIndex-1);
        m_firstIndex = 0;
        m_numberOfPoints = m_points.size();
}

void LineGraph::addPoint(float x, float y)
{
    m_dataSource->addPoint(x,y);
}

void LineGraph::setDataSource(LineGraphDataSource *dataSource)
{
    if (m_dataSource == dataSource)
        return;

    if(m_dataSource) {
        m_dataSource->disconnect(this);
    }

    m_dataSource = dataSource;
    connect(m_dataSource, SIGNAL(dataChanged()), this, SLOT(markDirty()));
    emit dataSourceChanged(dataSource);
}

void LineGraph::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
}

void LineGraph::setStyle(Qt::PenStyle style)
{
    if (m_style == style)
        return;

    m_style = style;
    emit styleChanged(style);
}

void LineGraph::setWidth(int width)
{
    if (m_width == width)
        return;

    m_width = width;
    emit widthChanged(width);
}

void LineGraphDataSource::addPoint(float x, float y)
{
    m_points.push_back(QPointF(x,y));
    if(++m_numberOfPoints > m_maxNumberOfPoints) {
        m_numberOfPoints = m_maxNumberOfPoints;
        m_firstIndex++;
    }

    if(m_points.size() > 2*m_maxNumberOfPoints) {
        cleanupMemory();
    }

    emit dataChanged();
}
