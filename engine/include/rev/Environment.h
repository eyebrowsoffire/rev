#pragma once

#include "rev/Types.h"

#include <memory>
#include <vector>

namespace rev {
class IActor;

class Environment {
public:
  Environment();

  void addActor(std::shared_ptr<IActor> actor);

  void play();
  void pause();

  void tick();

  void kill();
  bool isDead() const;

  Duration getTotalElapsedTime() const;

private:
  TimePoint _lastTick;
  Duration _elapsedTime;

  std::vector<std::shared_ptr<IActor>> _actors;

  bool _dead;
  bool _paused;
};
} // namespace rev