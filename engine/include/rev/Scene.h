#pragma once

#include "rev/gl/Uniform.h"

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <set>

namespace rev
{
class Model;
class SceneObject;

class Scene
{
public:
    std::shared_ptr<SceneObject> addObject(std::shared_ptr<Model> model);
    void renderAllObjects(Uniform<glm::mat4>& transformUniform);
private:
    std::map<std::shared_ptr<Model>, std::set<std::shared_ptr<SceneObject>>> _objectsByModel;
};

} // namespace rev