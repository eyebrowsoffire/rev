#pragma once

#include "rev/IModel.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"

#include <glm/glm.hpp>

namespace rev
{

struct VertexData
{
    glm::vec3 position;
    glm::vec3 normal;
};

class MaterialProperties
{
    glm::vec3 ambientColor;
    glm::vec3 emissiveColor;
    glm::vec3 specularColor;
    glm::vec3 diffuseColor;
    float specularExponent;
};

class ModelComponent
{
  public:
    ModelComponent(gsl::span<GLuint> indexes, MaterialProperties &materials)
    {
    }

  private:
    Buffer _indexes;
};

class CompositeModel : public IModel
{
  public:
    CompositeModel();

  private:
    struct SubModel
    {
        Buffer indexes;
    };
    VertexArray _vao;
    std::vector<ModelComponent>
};
} // namespace rev