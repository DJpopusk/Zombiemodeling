#pragma once

#include <QObject>
#include <QRectF>
#include <memory>
#include <random>
#include <vector>

#include "worldobject.h"

class World : public QObject
{
    Q_OBJECT
public:
    explicit World(QObject *parent = nullptr);

    void reset(int humans, int zombies);

    void setBounds(const QRectF &rect);
    QRectF bounds() const;

    void setDefaultBiteRadius(double radius);
    double defaultBiteRadius() const;

    void step(double dt);

    const std::vector<std::unique_ptr<WorldObject>> &objects() const;
    double time() const;
    int humanCount() const;
    int zombieCount() const;

    WorldObject *closestHuman(const QPointF &pos) const;
    std::vector<WorldObject *> objectsInRadius(const QPointF &pos, double radius, ObjType type) const;

signals:
    void populationChanged(int humans, int zombies, double time);
    void worldUpdated();

private slots:
    void onBite(WorldObject *victim);

private:
    void spawnHumans(int count);
    void spawnZombies(int count);
    void connectObject(WorldObject *obj);
    void processPendingConversions();

    std::mt19937 m_rng;
    QRectF m_bounds{0.0, 0.0, 120.0, 80.0};
    std::vector<std::unique_ptr<WorldObject>> m_objects;
    std::vector<WorldObject *> m_pendingConversions;
    double m_time{0.0};
    double m_defaultBiteRadius{6.0};
};
