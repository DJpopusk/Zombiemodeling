#include "world.h"

#include "human.h"
#include "zombie.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
double length(const QPointF &p)
{
    return std::hypot(p.x(), p.y());
}

QPointF normalized(const QPointF &p)
{
    const double len = length(p);
    if (len < 1e-6)
    {
        return QPointF(1.0, 0.0);
    }
    return QPointF(p.x() / len, p.y() / len);
}
}

World::World(QObject *parent) : QObject(parent)
{
    std::random_device rd;
    m_rng.seed(rd());
}

void World::setBounds(const QRectF &rect)
{
    m_bounds = rect;
}

QRectF World::bounds() const
{
    return m_bounds;
}

void World::setDefaultBiteRadius(double radius)
{
    m_defaultBiteRadius = radius;
}

double World::defaultBiteRadius() const
{
    return m_defaultBiteRadius;
}

void World::reset(int humans, int zombies)
{
    m_objects.clear();
    m_pendingConversions.clear();
    m_time = 0.0;

    spawnHumans(humans);
    spawnZombies(zombies);

    emit populationChanged(humanCount(), zombieCount(), m_time);
    emit worldUpdated();
}

void World::spawnHumans(int count)
{
    std::uniform_real_distribution<double> distX(m_bounds.left(), m_bounds.right());
    std::uniform_real_distribution<double> distY(m_bounds.top(), m_bounds.bottom());
    std::uniform_real_distribution<double> dir(-1.0, 1.0);

    for (int i = 0; i < count; ++i)
    {
        auto human = std::make_unique<Human>();
        ObjState &s = human->mutableState();
        s.pos = QPointF(distX(m_rng), distY(m_rng));

        QPointF heading(dir(m_rng), dir(m_rng));
        heading = normalized(heading);
        s.vel = QPointF(heading.x() * 8.0, heading.y() * 8.0);

        connectObject(human.get());
        m_objects.push_back(std::move(human));
    }
}

void World::spawnZombies(int count)
{
    std::uniform_real_distribution<double> distX(m_bounds.left(), m_bounds.right());
    std::uniform_real_distribution<double> distY(m_bounds.top(), m_bounds.bottom());
    std::uniform_real_distribution<double> dir(-1.0, 1.0);

    for (int i = 0; i < count; ++i)
    {
        auto zombie = std::make_unique<Zombie>();
        ObjState &s = zombie->mutableState();
        s.pos = QPointF(distX(m_rng), distY(m_rng));

        QPointF heading(dir(m_rng), dir(m_rng));
        heading = normalized(heading);
        s.vel = QPointF(heading.x() * 4.0, heading.y() * 4.0);

        zombie->setBiteRadius(m_defaultBiteRadius);

        connectObject(zombie.get());
        m_objects.push_back(std::move(zombie));
    }
}

void World::connectObject(WorldObject *obj)
{
    if (auto *z = dynamic_cast<Zombie *>(obj))
    {
        connect(z, &Zombie::biteSignal, this, &World::onBite);
    }
}

WorldObject *World::closestHuman(const QPointF &pos) const
{
    WorldObject *closest = nullptr;
    double bestDist = std::numeric_limits<double>::max();

    for (const auto &obj : m_objects)
    {
        if (obj->type() != ObjType::Human)
        {
            continue;
        }
        const double d = length(obj->state().pos - pos);
        if (d < bestDist)
        {
            bestDist = d;
            closest = obj.get();
        }
    }

    return closest;
}

std::vector<WorldObject *> World::objectsInRadius(const QPointF &pos, double radius, ObjType type) const
{
    std::vector<WorldObject *> result;
    for (const auto &obj : m_objects)
    {
        if (obj->type() != type)
        {
            continue;
        }
        if (length(obj->state().pos - pos) <= radius)
        {
            result.push_back(obj.get());
        }
    }
    return result;
}

void World::onBite(WorldObject *victim)
{
    if (victim == nullptr || victim->type() != ObjType::Human)
    {
        return;
    }
    const bool alreadyQueued = std::any_of(m_pendingConversions.begin(), m_pendingConversions.end(),
                                           [&](WorldObject *ptr) { return ptr == victim; });
    if (!alreadyQueued)
    {
        m_pendingConversions.push_back(victim);
    }
}

void World::processPendingConversions()
{
    for (WorldObject *victim : m_pendingConversions)
    {
        auto it = std::find_if(m_objects.begin(), m_objects.end(),
                               [&](const std::unique_ptr<WorldObject> &ptr) { return ptr.get() == victim; });
        if (it == m_objects.end() || (*it)->type() != ObjType::Human)
        {
            continue;
        }

        const ObjState saved = (*it)->state();
        m_objects.erase(it);

        auto zombie = std::make_unique<Zombie>();
        zombie->mutableState().pos = saved.pos;
        zombie->mutableState().vel = saved.vel;
        zombie->setBiteRadius(m_defaultBiteRadius);
        connectObject(zombie.get());
        m_objects.push_back(std::move(zombie));
    }

    m_pendingConversions.clear();
}

void World::step(double dt)
{
    m_time += dt;

    for (auto &obj : m_objects)
    {
        obj->updateState(*this, dt);
    }

    for (auto &obj : m_objects)
    {
        obj->integrate(dt, m_bounds);
    }

    processPendingConversions();

    for (auto &obj : m_objects)
    {
        obj->setBusy(false);
    }

    emit populationChanged(humanCount(), zombieCount(), m_time);
    emit worldUpdated();
}

const std::vector<std::unique_ptr<WorldObject>> &World::objects() const
{
    return m_objects;
}

double World::time() const
{
    return m_time;
}

int World::humanCount() const
{
    return std::count_if(m_objects.begin(), m_objects.end(),
                         [](const std::unique_ptr<WorldObject> &ptr) { return ptr->type() == ObjType::Human; });
}

int World::zombieCount() const
{
    return std::count_if(m_objects.begin(), m_objects.end(),
                         [](const std::unique_ptr<WorldObject> &ptr) { return ptr->type() == ObjType::Zombie; });
}
