#include "rev/SceneObject.h"

#include <glm/ext.hpp>

namespace rev
{

SceneObject::SceneObject(std::shared_ptr<IModel> model)
    : _model(std::move(model)), 
      _transform(glm::mat4()) {}

const glm::mat4 &SceneObject::getTransform() const { return _transform; }

void SceneObject::translate(const glm::vec3 &tVector)
{
    _transform = glm::translate(_transform, tVector);
}

} // namespace rev
