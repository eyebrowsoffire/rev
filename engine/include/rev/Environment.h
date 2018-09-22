#pragma once

#include <memory>

namespace rev
{
class Actor;

class Environment
{
public:
    void play();
    void pause();

    void tick();

    void kill();
    bool isDead() const;
private:
    bool _dead = false;
    bool _paused = false;
};
} // namespace rev