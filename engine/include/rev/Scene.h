#pragma once

#include "rev/SceneObjectGroup.h"
#include "rev/gl/Uniform.h"

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace rev {
class Light;
class SceneObject;

class Scene {
public:
    void renderAllObjects(Camera& camera);
    void renderAllLights(Camera& camera);

    void addObjectGroup(std::shared_ptr<ISceneObjectGroup> group);
    void addLightGroup(std::shared_ptr<ISceneObjectGroup> group);

private:
    std::vector<std::shared_ptr<ISceneObjectGroup>> _objectGroups;
    std::vector<std::shared_ptr<ISceneObjectGroup>> _lightGroups;
};

} // namespace rev