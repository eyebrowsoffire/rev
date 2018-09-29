#include "rev/Scene.h"

#include "rev/Light.h"
#include "rev/Model.h"
#include "rev/SceneObject.h"

namespace rev
{
std::shared_ptr<SceneObject> Scene::addObject(std::shared_ptr<Model> model)
{
    auto object = std::make_shared<SceneObject>(model);
    _objectsByModel[model].insert(object);
    return std::move(object);
}

std::shared_ptr<Light> Scene::addLight()
{
    auto light = std::make_shared<Light>();
    _lights.insert(light);
    return std::move(light);
}

void Scene::renderAllObjects(Uniform<glm::mat4> &transformUniform,
                             Uniform<glm::vec3> &baseColor)
{
    for (const auto &pair : _objectsByModel)
    {
        auto &model = pair.first;
        VertexArrayContext modelContext = model->getContext();

        baseColor.set(model->getBaseColor());

        for (const auto &object : pair.second)
        {
            transformUniform.set(object->getTransform());
            glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        }
    }
}

void Scene::renderAllLights(Uniform<glm::vec3> &lightPositionUniform,
                     Uniform<glm::vec3> &lightBaseColor)
{
    for (const auto & light : _lights)
    {
        lightPositionUniform.set(light->getPosition());
        lightBaseColor.set(light->getBaseColor());

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

} // namespace rev
