#pragma once

#include "rev/Types.h"

#include <memory>

namespace rev {
class Environment;

class IActor {
public:
    ~IActor() = default;

    virtual void tick(Environment& environment, Duration elapsedTime) = 0;
    virtual void kill(Environment& environment) = 0;
};

} // namespace rev