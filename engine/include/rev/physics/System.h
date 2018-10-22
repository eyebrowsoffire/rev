#pragma once

#include "rev/IActor.h"

namespace rev::physics
{
class System : public IActor
{
  public:
    // IActor
    virtual void tick(Environment &environment, Duration elapsedTime) override;
    virtual void kill(Environment &environment) override;
};
} // namespace rev