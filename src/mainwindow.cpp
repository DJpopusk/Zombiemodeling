#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <algorithm>

MainWindow::MainWindow()
    : ui(std::make_unique<Ui::MainWindow>())
{
    setupUi();
    setupPlots();

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    connect(&m_world, &World::populationChanged, this, &MainWindow::onPopulationChanged);

    resetWorldFromInputs();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    ui->setupUi(this);

    connect(ui->initButton, &QPushButton::clicked, this, &MainWindow::onInit);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStart);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::onPause);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStop);
}

void MainWindow::setupPlots()
{
    ui->worldPlot->clearGraphs();
    auto *humansGraph = ui->worldPlot->addGraph();
    humansGraph->setLineStyle(QCPGraph::lsNone);
    humansGraph->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::blue), QBrush(Qt::blue), 7.0));

    auto *zombiesGraph = ui->worldPlot->addGraph();
    zombiesGraph->setLineStyle(QCPGraph::lsNone);
    const QColor zombieColor(0, 90, 0);
    zombiesGraph->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(zombieColor), QBrush(zombieColor), 9.0));

    ui->worldPlot->xAxis->setLabel(QString());
    ui->worldPlot->yAxis->setLabel(QString());
    ui->worldPlot->setBackground(Qt::white);

    ui->historyPlot->clearGraphs();
    auto *humanLine = ui->historyPlot->addGraph();
    humanLine->setPen(QPen(Qt::blue, 2.0));
    humanLine->setLineStyle(QCPGraph::lsLine);

    auto *zombieLine = ui->historyPlot->addGraph();
    zombieLine->setPen(QPen(QColor(0, 90, 0), 2.0));
    zombieLine->setLineStyle(QCPGraph::lsLine);

    ui->historyPlot->xAxis->setLabel(QString());
    ui->historyPlot->yAxis->setLabel(QStringLiteral("N"));
}

void MainWindow::resetWorldFromInputs()
{
    m_timer.stop();
    m_timeHistory.clear();
    m_humanHistory.clear();
    m_zombieHistory.clear();

    m_world.setDefaultBiteRadius(ui->biteRadiusSpin->value());
    m_world.reset(ui->humansSpin->value(), ui->zombiesSpin->value());

    refreshWorldPlot();
    refreshHistoryPlot();
}

void MainWindow::onInit()
{
    resetWorldFromInputs();
}

void MainWindow::onStart()
{
    if (!m_timer.isActive())
    {
        m_world.setDefaultBiteRadius(ui->biteRadiusSpin->value());
        m_timer.start(60);
    }
}

void MainWindow::onPause()
{
    m_timer.stop();
}

void MainWindow::onStop()
{
    m_timer.stop();
    refreshHistoryPlot();
}

void MainWindow::onTick()
{
    m_world.step(ui->dtSpin->value());
    refreshWorldPlot();
}

void MainWindow::onPopulationChanged(int humans, int zombies, double time)
{
    appendHistory(humans, zombies, time);
    updateStatusLabel(humans, zombies, time);
    refreshHistoryPlot();
}

void MainWindow::appendHistory(int humans, int zombies, double time)
{
    m_timeHistory.append(time);
    m_humanHistory.append(humans);
    m_zombieHistory.append(zombies);
}

void MainWindow::updateStatusLabel(int humans, int zombies, double time)
{
    ui->statusLabel->setText(
        QStringLiteral("t=%1 | люди=%2 | зомби=%3").arg(time, 0, 'f', 2).arg(humans).arg(zombies));
}

void MainWindow::refreshWorldPlot()
{
    QVector<double> humanX;
    QVector<double> humanY;
    QVector<double> zombieX;
    QVector<double> zombieY;

    for (const auto &obj : m_world.objects())
    {
        const auto &s = obj->state();
        if (obj->type() == ObjType::Human)
        {
            humanX.append(s.pos.x());
            humanY.append(s.pos.y());
        }
        else if (obj->type() == ObjType::Zombie)
        {
            zombieX.append(s.pos.x());
            zombieY.append(s.pos.y());
        }
    }

    if (auto *g = ui->worldPlot->graph(0))
    {
        g->setData(humanX, humanY);
    }
    if (auto *g = ui->worldPlot->graph(1))
    {
        g->setData(zombieX, zombieY);
    }

    const QRectF b = m_world.bounds();
    ui->worldPlot->xAxis->setRange(b.left(), b.right());
    ui->worldPlot->yAxis->setRange(b.top(), b.bottom());
    ui->worldPlot->replot();
}

void MainWindow::refreshHistoryPlot()
{
    if (auto *g = ui->historyPlot->graph(0))
    {
        g->setData(m_timeHistory, m_humanHistory);
    }
    if (auto *g = ui->historyPlot->graph(1))
    {
        g->setData(m_timeHistory, m_zombieHistory);
    }

    const double lastT = m_timeHistory.isEmpty() ? 1.0 : m_timeHistory.last();
    int maxPop = 1;
    for (int v : m_humanHistory)
    {
        maxPop = std::max(maxPop, v);
    }
    for (int v : m_zombieHistory)
    {
        maxPop = std::max(maxPop, v);
    }

    ui->historyPlot->xAxis->setRange(0.0, std::max(1.0, lastT));
    ui->historyPlot->yAxis->setRange(0.0, static_cast<double>(maxPop) * 1.1 + 1e-3);
    ui->historyPlot->replot();
}
