#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <memory>

#include "world.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private slots:
    void onInit();
    void onStart();
    void onPause();
    void onStop();
    void onTick();
    void onPopulationChanged(int humans, int zombies, double time);

private:
    void setupUi();
    void setupPlots();
    void resetWorldFromInputs();
    void refreshWorldPlot();
    void refreshHistoryPlot();
    void appendHistory(int humans, int zombies, double time);
    void updateStatusLabel(int humans, int zombies, double time);

    std::unique_ptr<Ui::MainWindow> ui;

    QTimer m_timer;
    World m_world;

    QVector<double> m_timeHistory;
    QVector<double> m_humanHistory;
    QVector<double> m_zombieHistory;
};
