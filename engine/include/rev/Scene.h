#pragma once

#include "rev/gl/Uniform.h"

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <set>

namespace rev
{
class Light;
class IModel;
class SceneObject;

class Scene
{
public:
    std::shared_ptr<SceneObject> addObject(std::shared_ptr<IModel> model);
    std::shared_ptr<Light> addLight();

    void renderAllObjects(Uniform<glm::mat4>& transformUniform, Uniform<glm::vec3>& baseColorUniform);
    void renderAllLights(Uniform<glm::vec3>& lightPositionUniform, Uniform<glm::vec3>& lightBaseColor);
private:
    std::map<std::shared_ptr<IModel>, std::set<std::shared_ptr<SceneObject>>> _objectsByModel;
    std::set<std::shared_ptr<Light>> _lights;
};

} // namespace rev