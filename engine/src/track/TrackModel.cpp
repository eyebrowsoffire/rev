#include "rev/track/TrackModel.h"

#include "rev/MaterialProperties.h"
#include "rev/Mesh.h"

#include <vector>

namespace rev {

namespace {
    constexpr MaterialProperties kMaterialProperties{
        glm::vec3(0.763975f, 0.763975f, 0.763975f),
        glm::vec3(0.000000f, 0.000000f, 0.000000f),
        glm::vec3(0.114541f, 0.101377f, 0.288820f),
        glm::vec3(0.6f, 0.6f, 0.6f),
        445.098039f,
    };
}

TrackModel::TrackModel(ShaderLibrary& library, Mesh trackMesh)
    : _program(library.acquireProgram(
          std::array{ VertexShaderComponentInfo::make<DrawMaterialsVertexComponent>() },
          std::array{ FragmentShaderComponentInfo::make<DrawMaterialsFragmentComponent>() }))
    , _trackMesh(std::move(trackMesh))
    , _model(_program->getUniform<glm::mat4>("model"))
    , _view(_program->getUniform<glm::mat4>("view"))
    , _projection(_program->getUniform<glm::mat4>("projection"))
    , _ambient(_program->getUniform<glm::vec3>("fAmbient"))
    , _emissive(_program->getUniform<glm::vec3>("fEmissive"))
    , _diffuse(_program->getUniform<glm::vec3>("fDiffuse"))
    , _specular(_program->getUniform<glm::vec3>("fSpecular"))
    , _specularExponent(_program->getUniform<float>("fSpecularExponent"))
{
}

void TrackModel::render(Camera& camera, gsl::span<std::shared_ptr<TrackObject>>)
{
    auto context = _program->prepareContext();
    bindMaterialProperties(kMaterialProperties);
    _model.set(glm::mat4(1.0));
    _view.set(camera.getViewMatrix());
    _projection.set(camera.getProjectionMatrix());

    auto vaoContext = _trackMesh.getContext();
    _trackMesh.drawVertices();
}

void TrackModel::bindMaterialProperties(const MaterialProperties& properties)
{
    _ambient.set(properties.ambientColor);
    _emissive.set(properties.emissiveColor);
    _diffuse.set(properties.diffuseColor);
    _specular.set(properties.specularColor);
    _specularExponent.set(properties.specularExponent);
}

}
