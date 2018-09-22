#include "rev/Environment.h"

namespace rev {
    void Environment::play()
    {
        _paused = false;
    }

    void Environment::pause()
    {
        _paused = true;
    }

    void Environment::tick()
    {
        if(_paused || _dead)
        {
            return;
        }
    }

    void Environment::kill()
    {
        _dead = true;
    }

    bool Environment::isDead() const
    {
        return _dead;
    }
}
