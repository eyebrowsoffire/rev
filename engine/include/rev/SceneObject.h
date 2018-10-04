#pragma once

#include "rev/IModel.h"
#include <glm/glm.hpp>

#include <memory>

namespace rev
{

class SceneObject
{
  public:
    SceneObject(std::shared_ptr<IModel> model);

    const glm::mat4 &getTransform() const;
    void translate(const glm::vec3 &);

  private:
    std::shared_ptr<IModel> _model;
    glm::mat4 _transform;
};

} // namespace rev