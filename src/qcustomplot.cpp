#include "qcustomplot.h"

#include <QPainter>
#include <QtMath>
#include <limits>

QCPScatterStyle::QCPScatterStyle(QCPScatterStyle::ScatterShape shape, const QPen &pen,
                                 const QBrush &brush, double size)
    : m_shape(shape), m_pen(pen), m_brush(brush), m_size(size)
{
}

void QCPScatterStyle::setShape(QCPScatterStyle::ScatterShape shape)
{
    m_shape = shape;
}

QCPScatterStyle::ScatterShape QCPScatterStyle::shape() const
{
    return m_shape;
}

void QCPScatterStyle::setPen(const QPen &pen)
{
    m_pen = pen;
}

const QPen &QCPScatterStyle::pen() const
{
    return m_pen;
}

void QCPScatterStyle::setBrush(const QBrush &brush)
{
    m_brush = brush;
}

const QBrush &QCPScatterStyle::brush() const
{
    return m_brush;
}

void QCPScatterStyle::setSize(double size)
{
    m_size = size;
}

double QCPScatterStyle::size() const
{
    return m_size;
}

QCPGraph::QCPGraph() = default;

void QCPGraph::setData(const QVector<double> &x, const QVector<double> &y)
{
    m_x = x;
    m_y = y;
}

const QVector<double> &QCPGraph::dataX() const
{
    return m_x;
}

const QVector<double> &QCPGraph::dataY() const
{
    return m_y;
}

void QCPGraph::setPen(const QPen &pen)
{
    m_pen = pen;
}

const QPen &QCPGraph::pen() const
{
    return m_pen;
}

void QCPGraph::setBrush(const QBrush &brush)
{
    m_brush = brush;
}

const QBrush &QCPGraph::brush() const
{
    return m_brush;
}

void QCPGraph::setLineStyle(QCPGraph::LineStyle style)
{
    m_lineStyle = style;
}

QCPGraph::LineStyle QCPGraph::lineStyle() const
{
    return m_lineStyle;
}

void QCPGraph::setScatterStyle(const QCPScatterStyle &style)
{
    m_scatterStyle = style;
}

const QCPScatterStyle &QCPGraph::scatterStyle() const
{
    return m_scatterStyle;
}

QCustomPlot::QCustomPlot(QWidget *parent) : QWidget(parent), xAxis(new QCPAxis), yAxis(new QCPAxis)
{
    setMinimumSize(320, 200);
}

QCustomPlot::~QCustomPlot()
{
    delete xAxis;
    delete yAxis;
}

QCPGraph *QCustomPlot::addGraph()
{
    m_graphs.push_back(std::make_unique<QCPGraph>());
    return m_graphs.back().get();
}

QCPGraph *QCustomPlot::graph(int index) const
{
    if (index < 0 || static_cast<size_t>(index) >= m_graphs.size())
    {
        return nullptr;
    }
    return m_graphs[static_cast<size_t>(index)].get();
}

int QCustomPlot::graphCount() const
{
    return static_cast<int>(m_graphs.size());
}

void QCustomPlot::clearGraphs()
{
    m_graphs.clear();
}

void QCustomPlot::replot()
{
    update();
}

void QCustomPlot::rescaleAxes()
{
    if (m_graphs.empty())
    {
        return;
    }

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto &g : m_graphs)
    {
        const auto &xs = g->dataX();
        const auto &ys = g->dataY();
        for (int i = 0; i < xs.size() && i < ys.size(); ++i)
        {
            minX = std::min(minX, xs[i]);
            maxX = std::max(maxX, xs[i]);
            minY = std::min(minY, ys[i]);
            maxY = std::max(maxY, ys[i]);
        }
    }

    if (minX == std::numeric_limits<double>::max() || maxX == std::numeric_limits<double>::lowest())
    {
        minX = 0.0;
        maxX = 1.0;
    }
    if (minY == std::numeric_limits<double>::max() || maxY == std::numeric_limits<double>::lowest())
    {
        minY = 0.0;
        maxY = 1.0;
    }

    const double xMargin = (maxX - minX) * 0.05 + 1e-3;
    const double yMargin = (maxY - minY) * 0.05 + 1e-3;

    xAxis->setRange(minX - xMargin, maxX + xMargin);
    yAxis->setRange(minY - yMargin, maxY + yMargin);
}

void QCustomPlot::setBackground(const QBrush &brush)
{
    m_background = brush;
}

QRectF QCustomPlot::plotRect() const
{
    const int left = 55;
    const int top = 20;
    const int right = 20;
    const int bottom = 45;
    return QRectF(rect().adjusted(left, top, -right, -bottom));
}

void QCustomPlot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), m_background);

    const QRectF pr = plotRect();

    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(pr);

    const double xLower = xAxis->lower();
    const double xUpper = xAxis->upper();
    const double yLower = yAxis->lower();
    const double yUpper = yAxis->upper();

    const double xSpan = (qFuzzyCompare(xLower, xUpper)) ? 1.0 : (xUpper - xLower);
    const double ySpan = (qFuzzyCompare(yLower, yUpper)) ? 1.0 : (yUpper - yLower);

    auto toScreen = [&](double x, double y) -> QPointF {
        const double sx = pr.left() + (x - xLower) / xSpan * pr.width();
        const double sy = pr.bottom() - (y - yLower) / ySpan * pr.height();
        return {sx, sy};
    };

    for (const auto &g : m_graphs)
    {
        const auto &xs = g->dataX();
        const auto &ys = g->dataY();
        if (xs.isEmpty() || ys.isEmpty())
        {
            continue;
        }

        painter.setPen(g->pen());
        painter.setBrush(g->brush());

        if (g->lineStyle() == QCPGraph::lsLine && xs.size() > 1)
        {
            QPolygonF poly;
            poly.reserve(xs.size());
            for (int i = 0; i < xs.size() && i < ys.size(); ++i)
            {
                poly.append(toScreen(xs[i], ys[i]));
            }
            painter.drawPolyline(poly);
        }

        const auto &scatter = g->scatterStyle();
        if (scatter.shape() != QCPScatterStyle::ssNone)
        {
            painter.setPen(scatter.pen());
            painter.setBrush(scatter.brush());
            const double radius = scatter.size() / 2.0;
            for (int i = 0; i < xs.size() && i < ys.size(); ++i)
            {
                const QPointF p = toScreen(xs[i], ys[i]);
                switch (scatter.shape())
                {
                case QCPScatterStyle::ssCircle:
                    painter.drawEllipse(p, radius, radius);
                    break;
                case QCPScatterStyle::ssSquare:
                    painter.drawRect(QRectF(p.x() - radius, p.y() - radius, radius * 2, radius * 2));
                    break;
                default:
                    break;
                }
            }
        }
    }

    painter.setPen(Qt::black);
    painter.drawText(QPointF(pr.center().x(), rect().bottom() - 12), xAxis->label());

    painter.save();
    painter.translate(15, pr.center().y());
    painter.rotate(-90);
    painter.drawText(QPointF(0, 0), yAxis->label());
    painter.restore();

    auto drawTick = [&](double value, bool isX) {
        const QString text = QString::number(value, 'f', 1);
        if (isX)
        {
            const QPointF p = toScreen(value, yLower);
            painter.drawLine(QPointF(p.x(), pr.bottom()), QPointF(p.x(), pr.bottom() + 4));
            painter.drawText(QPointF(p.x() - 12, pr.bottom() + 16), text);
        }
        else
        {
            const QPointF p = toScreen(xLower, value);
            painter.drawLine(QPointF(pr.left() - 4, p.y()), QPointF(pr.left(), p.y()));
            painter.drawText(QPointF(4, p.y() + 4), text);
        }
    };

    drawTick(xLower, true);
    drawTick(xUpper, true);
    drawTick((xLower + xUpper) * 0.5, true);
    drawTick(yLower, false);
    drawTick(yUpper, false);
    drawTick((yLower + yUpper) * 0.5, false);
}

QCPAxis::QCPAxis() = default;

void QCPAxis::setRange(double lower, double upper)
{
    m_lower = lower;
    m_upper = upper;
}

double QCPAxis::lower() const
{
    return m_lower;
}

double QCPAxis::upper() const
{
    return m_upper;
}

void QCPAxis::setLabel(const QString &label)
{
    m_label = label;
}

const QString &QCPAxis::label() const
{
    return m_label;
}
