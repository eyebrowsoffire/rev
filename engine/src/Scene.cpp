#include "rev/Scene.h"

#include "rev/Light.h"
#include "rev/IModel.h"

namespace rev
{

std::shared_ptr<Light> Scene::addPointLight()
{
    auto light = std::make_shared<Light>(Light::Type::Point);
    _pointLights.insert(light);
    return std::move(light);
}

std::shared_ptr<Light> Scene::addDirectionalLight()
{
    auto light = std::make_shared<Light>(Light::Type::Directional);
    _directionalLights.insert(light);
    return std::move(light);
}

void Scene::addObjectGroup(std::shared_ptr<ISceneObjectGroup> group)
{
    _objectGroups.push_back(std::move(group));
}

void Scene::renderAllObjects(Camera &camera)
{
    for (const auto &objectGroup : _objectGroups)
    {
        objectGroup->render(camera);
    }
}

void Scene::renderAllDirectionalLights(Uniform<glm::vec3> &lightDirectionUniform,
                    Uniform<glm::vec3> &lightBaseColor)
{
    for (const auto & light : _directionalLights)
    {
        lightDirectionUniform.set(light->getDirection());
        lightBaseColor.set(light->getBaseColor());

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}             

void Scene::renderAllPointLights(Uniform<glm::vec3> &lightPositionUniform,
                     Uniform<glm::vec3> &lightBaseColor)
{
    for (const auto & light : _pointLights)
    {
        lightPositionUniform.set(light->getPosition());
        lightBaseColor.set(light->getBaseColor());

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

} // namespace rev
