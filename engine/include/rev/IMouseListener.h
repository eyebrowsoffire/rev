#pragma once

#include "rev/Types.h"

namespace rev {

enum class MouseButton
{
    Left,
    Right
};

class IMouseListener
{
public:
    ~IMouseListener() = default;

    virtual void buttonPressed(MouseButton button) = 0;
    virtual void buttonReleased(MouseButton button) = 0;
    virtual void moved(const Point<double> newPosition) = 0;
    virtual void scrolled(double xOffset, double yOffset) = 0;
};

}