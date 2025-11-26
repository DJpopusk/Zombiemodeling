#include "zombie.h"

#include "world.h"

#include <QRandomGenerator>
#include <QtMath>

Zombie::Zombie(QObject *parent) : WorldObject(ObjType::Zombie, parent) {}

void Zombie::setBiteRadius(double radius)
{
    m_biteRadius = radius;
}

double Zombie::biteRadius() const
{
    return m_biteRadius;
}

void Zombie::wander()
{
    const double jitter = 3.0;
    auto *rng = QRandomGenerator::global();
    const double dx = (rng->generateDouble() * 2.0 - 1.0) * jitter;
    const double dy = (rng->generateDouble() * 2.0 - 1.0) * jitter;

    QPointF vel = m_state.vel + QPointF(dx, dy);
    const double len = std::hypot(vel.x(), vel.y());
    if (len > 1e-3)
    {
        const double scale = m_speed / len;
        vel.setX(vel.x() * scale);
        vel.setY(vel.y() * scale);
    }

    m_state.vel = vel;
}

void Zombie::updateState(World &world, double dt)
{
    Q_UNUSED(dt)

    WorldObject *target = world.closestHuman(m_state.pos);
    if (target != nullptr)
    {
        const QPointF diff = target->state().pos - m_state.pos;
        const double distance = std::hypot(diff.x(), diff.y());

        if (distance <= m_biteRadius)
        {
            if (!m_busy)
            {
                emit biteSignal(target);
                m_busy = true;
            }
            m_state.vel = QPointF(0.0, 0.0);
            return;
        }

        if (distance > 1e-3)
        {
            const double scale = m_speed / distance;
            m_state.vel = QPointF(diff.x() * scale, diff.y() * scale);
        }
    }
    else
    {
        wander();
    }
}
