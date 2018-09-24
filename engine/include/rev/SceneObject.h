#pragma once

#include "rev/Model.h"
#include <glm/glm.hpp>

namespace rev
{

class SceneObject
{
  public:
    SceneObject(std::shared_ptr<Model> model);

    const glm::mat4 &getTransform() const;
    void translate(const glm::vec3 &);

  private:
    std::shared_ptr<Model> _model;
    glm::mat4 _transform;
};

} // namespace rev