#include "worldobject.h"

#include <algorithm>

WorldObject::WorldObject(ObjType type, QObject *parent) : QObject(parent), m_type(type) {}

ObjType WorldObject::type() const
{
    return m_type;
}

const ObjState &WorldObject::state() const
{
    return m_state;
}

ObjState &WorldObject::mutableState()
{
    return m_state;
}

bool WorldObject::isBusy() const
{
    return m_busy;
}

void WorldObject::setBusy(bool busy)
{
    m_busy = busy;
}

void WorldObject::integrate(double dt, const QRectF &bounds)
{
    QPointF next(m_state.pos.x() + m_state.vel.x() * dt, m_state.pos.y() + m_state.vel.y() * dt);

    if (next.x() < bounds.left())
    {
        next.setX(bounds.left());
        m_state.vel.setX(-m_state.vel.x());
    }
    else if (next.x() > bounds.right())
    {
        next.setX(bounds.right());
        m_state.vel.setX(-m_state.vel.x());
    }

    if (next.y() < bounds.top())
    {
        next.setY(bounds.top());
        m_state.vel.setY(-m_state.vel.y());
    }
    else if (next.y() > bounds.bottom())
    {
        next.setY(bounds.bottom());
        m_state.vel.setY(-m_state.vel.y());
    }

    m_state.pos = next;
}
