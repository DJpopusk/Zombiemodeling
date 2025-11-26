#include "human.h"

#include <QRandomGenerator>
#include <QtMath>

Human::Human(QObject *parent) : WorldObject(ObjType::Human, parent) {}

void Human::updateState(World &world, double dt)
{
    Q_UNUSED(world)
    Q_UNUSED(dt)

    m_state.curStatus = ObjStatus::Moving;

    const double jitter = 4.0;
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
    else
    {
        vel = QPointF((rng->generateDouble() * 2.0 - 1.0) * m_speed,
                      (rng->generateDouble() * 2.0 - 1.0) * m_speed);
    }

    m_state.vel = vel;
}
