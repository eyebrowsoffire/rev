#include "rev/Scene.h"

namespace rev {

void Scene::addObjectGroup(std::shared_ptr<ISceneObjectGroup> group)
{
    _objectGroups.push_back(std::move(group));
}

void Scene::addLightGroup(std::shared_ptr<ISceneObjectGroup> group)
{
    _lightGroups.push_back(std::move(group));
}

void Scene::renderAllObjects(Camera& camera)
{
    for (const auto& objectGroup : _objectGroups) {
        objectGroup->render(camera);
    }
}

void Scene::renderAllLights(Camera& camera)
{
    for (const auto& lightGroup : _lightGroups) {
        lightGroup->render(camera);
    }
}

} // namespace rev
