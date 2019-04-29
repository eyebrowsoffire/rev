#pragma once

namespace rev {
enum class KeyboardKey { W, S, A, D, Space };

class IKeyboardListener {
public:
  virtual ~IKeyboardListener() = default;

  virtual void keyPressed(KeyboardKey key) = 0;
  virtual void keyReleased(KeyboardKey key) = 0;
};
} // namespace rev