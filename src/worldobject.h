#pragma once

#include <QObject>
#include <QPointF>
#include <QRectF>

class World;

enum class ObjType
{
    Human,
    Zombie
};

enum class ObjStatus
{
    Idle,
    Moving,
    Infected
};

struct ObjState
{
    ObjStatus curStatus{ObjStatus::Idle};
    QPointF pos{0.0, 0.0};
    QPointF vel{0.0, 0.0};
};

class WorldObject : public QObject
{
    Q_OBJECT
public:
    explicit WorldObject(ObjType type, QObject *parent = nullptr);
    ~WorldObject() override = default;

    ObjType type() const;
    const ObjState &state() const;
    ObjState &mutableState();

    bool isBusy() const;
    void setBusy(bool busy);

    virtual void updateState(World &world, double dt) = 0;
    void integrate(double dt, const QRectF &bounds);

protected:
    ObjType m_type;
    ObjState m_state;
    bool m_busy{false};
};
