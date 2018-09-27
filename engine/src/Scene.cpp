#include "rev/Scene.h"

#include "rev/Model.h"
#include "rev/SceneObject.h"

namespace rev
{
    std::shared_ptr<SceneObject> Scene::addObject(std::shared_ptr<Model> model)
    {
        auto object = std::make_shared<SceneObject>(model);
        _objectsByModel[model].insert(object);
        return object;
    }

    void Scene::renderAllObjects(Uniform<glm::mat4>& transformUniform)
    {
        for(const auto& pair : _objectsByModel)
        {
            auto& model = pair.first;
            VertexArrayContext modelContext = model->getContext();

            for (const auto& object : pair.second)
            {
                transformUniform.set(object->getTransform());
                glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
            }
        }
    }
}
