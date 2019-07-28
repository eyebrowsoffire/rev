#include "rev/track/TrackModel.h"

#include "rev/MaterialProperties.h"
#include "rev/Mesh.h"
#include "rev/ProgramFactory.h"

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

TrackModel::TrackModel(ProgramFactory& factory, Mesh trackMesh)
    : _program(factory.getProgram<DrawMaterialsProgram>())
    , _trackMesh(std::move(trackMesh))
{
}

void TrackModel::render(Camera& camera, gsl::span<std::shared_ptr<TrackObject>>)
{
    auto context = _program->prepareContext();
    _program->applyMaterialProperties(kMaterialProperties);
    _program->model.set(glm::mat4(1.0));
    _program->view.set(camera.getViewMatrix());
    _program->projection.set(camera.getProjectionMatrix());

    auto vaoContext = _trackMesh.getContext();
    _trackMesh.drawVertices();
}

}
