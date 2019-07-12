#pragma once

#include "rev/NurbsCurve.h"

#include <string>

namespace rev {

// This is a simple custom file format that describes a nurbs curve
class CurveFile {
public:
    CurveFile(const std::string& filePath);

    const NurbsCurve<glm::vec3>& getCurve();

private:
    NurbsCurve<glm::vec3> _curve;
};
}