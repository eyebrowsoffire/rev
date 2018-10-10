#pragma once

#include <glm/glm.hpp>
#include <sstream>

namespace rev
{
class IModel;

inline glm::vec2 getVec2(std::istringstream &stream)
{
    glm::vec2 vec;
    stream >> vec.x;
    stream >> vec.y;
    return vec;
}

inline glm::vec3 getVec3(std::istringstream &stream)
{
    glm::vec3 vec;
    stream >> vec.x;
    stream >> vec.y;
    stream >> vec.z;
    return vec;
}

std::shared_ptr<IModel> createModelFromWavefrontFile(const std::string &filePath);
} // namespace rev