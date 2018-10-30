#pragma once

#include "rev/IActor.h"
#include "rev/physics/Gravity.h"
#include "rev/physics/Particle.h"

#include <memory>
#include <vector>

namespace rev::physics
{
class System : public IActor
{
public:
  System();
  
  // IActor
  virtual void tick(Environment &environment, Duration elapsedTime) override;
  virtual void kill(Environment &environment) override;

  std::shared_ptr<Particle> addParticle();
  std::shared_ptr<Gravity> getGravity() const;
private:
  std::vector<std::shared_ptr<Particle>> _particles;
  std::shared_ptr<Gravity> _gravity;
};
} // namespace rev::physics