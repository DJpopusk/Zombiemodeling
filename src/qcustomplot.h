#pragma once

#include <QBrush>
#include <QPen>
#include <QString>
#include <QVector>
#include <QWidget>
#include <memory>
#include <vector>

class QCPAxis;

class QCPScatterStyle
{
public:
    enum ScatterShape
    {
        ssNone,
        ssCircle,
        ssSquare
    };

    explicit QCPScatterStyle(ScatterShape shape = ssNone, const QPen &pen = QPen(Qt::black),
                             const QBrush &brush = QBrush(Qt::NoBrush), double size = 6.0);

    void setShape(ScatterShape shape);
    ScatterShape shape() const;

    void setPen(const QPen &pen);
    const QPen &pen() const;

    void setBrush(const QBrush &brush);
    const QBrush &brush() const;

    void setSize(double size);
    double size() const;

private:
    ScatterShape m_shape;
    QPen m_pen;
    QBrush m_brush;
    double m_size;
};

class QCPGraph
{
public:
    enum LineStyle
    {
        lsNone,
        lsLine
    };

    QCPGraph();

    void setData(const QVector<double> &x, const QVector<double> &y);
    const QVector<double> &dataX() const;
    const QVector<double> &dataY() const;

    void setPen(const QPen &pen);
    const QPen &pen() const;

    void setBrush(const QBrush &brush);
    const QBrush &brush() const;

    void setLineStyle(LineStyle style);
    LineStyle lineStyle() const;

    void setScatterStyle(const QCPScatterStyle &style);
    const QCPScatterStyle &scatterStyle() const;

private:
    QVector<double> m_x;
    QVector<double> m_y;
    QPen m_pen{Qt::black};
    QBrush m_brush{Qt::NoBrush};
    LineStyle m_lineStyle{lsLine};
    QCPScatterStyle m_scatterStyle;
};

class QCustomPlot : public QWidget
{
    Q_OBJECT

public:
    explicit QCustomPlot(QWidget *parent = nullptr);
    ~QCustomPlot() override;

    QCPGraph *addGraph();
    QCPGraph *graph(int index) const;
    int graphCount() const;
    void clearGraphs();
    void replot();
    void rescaleAxes();

    QCPAxis *xAxis;
    QCPAxis *yAxis;

    void setBackground(const QBrush &brush);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QRectF plotRect() const;

    std::vector<std::unique_ptr<QCPGraph>> m_graphs;
    QBrush m_background{Qt::white};
};

class QCPAxis
{
public:
    QCPAxis();

    void setRange(double lower, double upper);
    double lower() const;
    double upper() const;

    void setLabel(const QString &label);
    const QString &label() const;

private:
    double m_lower{0.0};
    double m_upper{1.0};
    QString m_label;
};
