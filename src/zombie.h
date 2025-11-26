#pragma once

#include "worldobject.h"

class Zombie : public WorldObject
{
    Q_OBJECT
public:
    explicit Zombie(QObject *parent = nullptr);

    void setBiteRadius(double radius);
    double biteRadius() const;

    void updateState(World &world, double dt) override;

signals:
    void biteSignal(WorldObject *victim);

private:
    void wander();

    double m_biteRadius{6.0};
    double m_speed{8.0};
};
