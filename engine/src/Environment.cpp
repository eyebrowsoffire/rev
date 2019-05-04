#include "rev/Environment.h"

#include "rev/IActor.h"

namespace rev {
Environment::Environment()
    : _elapsedTime{}
    , _paused(true)
    , _dead(false)
{
}

void Environment::addActor(std::shared_ptr<IActor> actor)
{
    _actors.push_back(std::move(actor));
}

void Environment::play()
{
    if (!_paused) {
        return;
    }
    _paused = false;
    _lastTick = std::chrono::steady_clock::now();
}

void Environment::pause() { _paused = true; }

void Environment::tick()
{
    if (_paused || _dead) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    Duration elapsed = now - _lastTick;
    _elapsedTime += elapsed;

    for (const auto& actor : _actors) {
        actor->tick(*this, elapsed);
    }
    _lastTick = now;
}

void Environment::kill() { _dead = true; }

bool Environment::isDead() const { return _dead; }

Duration Environment::getTotalElapsedTime() const { return _elapsedTime; }
} // namespace rev
