#include "rev/Scene.h"

#include "rev/IModel.h"
#include "rev/Light.h"

namespace rev {

std::shared_ptr<Light> Scene::addLight()
{
    auto light = std::make_shared<Light>();
    _lights.insert(light);
    return std::move(light);
}

void Scene::addObjectGroup(std::shared_ptr<ISceneObjectGroup> group)
{
    _objectGroups.push_back(std::move(group));
}

void Scene::renderAllObjects(Camera& camera)
{
    for (const auto& objectGroup : _objectGroups) {
        objectGroup->render(camera);
    }
}

void Scene::renderAllLights(Uniform<glm::vec3>& lightPositionUniform,
    Uniform<glm::vec3>& lightBaseColor)
{
    for (const auto& light : _lights) {
        lightPositionUniform.set(light->getPosition());
        lightBaseColor.set(light->getBaseColor());

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

} // namespace rev
