#pragma once

#include "rev/gl/Uniform.h"
#include "rev/SceneObjectGroup.h"

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace rev
{
class Light;
class IModel;
class SceneObject;

class Scene
{
public:
    std::shared_ptr<Light> addPointLight();
    std::shared_ptr<Light> addDirectionalLight();

    void renderAllObjects(Camera &camera);
    void renderAllPointLights(Uniform<glm::vec3>& lightPositionUniform, Uniform<glm::vec3>& lightBaseColor);
    void renderAllDirectionalLights(Uniform<glm::vec3>& lightDirectionUniform, Uniform<glm::vec3>& lightBaseColor);

    void addObjectGroup(std::shared_ptr<ISceneObjectGroup> group);
private:
    std::vector<std::shared_ptr<ISceneObjectGroup>> _objectGroups;
    std::set<std::shared_ptr<Light>> _pointLights;
    std::set<std::shared_ptr<Light>> _directionalLights;
};

} // namespace rev