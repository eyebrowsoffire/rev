#pragma once

#include "rev/Camera.h"
#include "rev/DrawMaterialsProgram.h"
#include "rev/Mesh.h"
#include "rev/shaders/ShaderLibrary.h"

#include <vector>

namespace rev {

class ProgramFactory;

struct TrackObject {
};

class TrackModel {
public:
    using SceneObjectType = TrackObject;

    TrackModel(ShaderLibrary& library, Mesh trackMesh);

    void render(Camera& camera, gsl::span<std::shared_ptr<TrackObject>> objects);

private:
    void bindMaterialProperties(const MaterialProperties& properties);

    std::shared_ptr<Program> _program;

    Uniform<glm::mat4> _model;
    Uniform<glm::mat4> _view;
    Uniform<glm::mat4> _projection;

    Uniform<glm::vec3> _ambient;
    Uniform<glm::vec3> _emissive;
    Uniform<glm::vec3> _diffuse;
    Uniform<glm::vec3> _specular;
    Uniform<float> _specularExponent;

    Mesh _trackMesh;
};
}; // namespace rev