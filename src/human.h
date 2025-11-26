#pragma once

#include "worldobject.h"

class Human : public WorldObject
{
    Q_OBJECT
public:
    explicit Human(QObject *parent = nullptr);

    void updateState(World &world, double dt) override;

private:
    double m_speed{12.0};
};
