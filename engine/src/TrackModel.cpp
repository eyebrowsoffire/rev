#include "rev/TrackModel.h"

#include "rev/MaterialProperties.h"
#include "rev/ProgramFactory.h"

#include "rev/Mesh.h"
#include <vector>

namespace rev {

namespace {
    constexpr MaterialProperties kMaterialProperties{
        glm::vec3(0.763975, 0.763975, 0.763975),
        glm::vec3(0.000000, 0.000000, 0.000000),
        glm::vec3(0.114541, 0.101377, 0.288820),
        glm::vec3(0.6, 0.6, 0.6),
        445.098039
    };

    struct TrackVertexData {
        TrackVertexData(const glm::vec3& positionArg, const glm::vec3& normalArg)
            : position(positionArg)
            , normal(normalArg)
        {
        }

        glm::vec3 position;
        glm::vec3 normal;

        static void setupAttributes(VertexArrayContext& context)
        {
            context.setupVertexAttribute<decltype(position)>(0, offsetof(TrackVertexData, position), sizeof(TrackVertexData));
            context.setupVertexAttribute<decltype(normal)>(1, offsetof(TrackVertexData, normal), sizeof(TrackVertexData));
        }
    };

    Mesh buildTrackMesh(const NurbsCurve<glm::vec3>& curve, float width, size_t segments)
    {
        Expects(segments > 0);

        float start = curve.getStart();
        float end = curve.getEnd();
        float curveLength = end - start;
        float segmentLength = curveLength / static_cast<float>(segments);

        std::vector<glm::vec3> guidePoints;
        for (float current = start; current < end; current += segmentLength) {
            guidePoints.push_back(curve[current]);
        }

        MeshBuilder<TrackVertexData> builder;
        auto firstGuidePoint = guidePoints.begin();
        auto secondGuidePoint = firstGuidePoint + 1;
        auto guidePointsEnd = guidePoints.end();
        size_t previousLeftIndex;
        size_t previousRightIndex;

        bool first = true;
        while (secondGuidePoint != guidePointsEnd) {
            glm::vec3 guideDirection = *secondGuidePoint - *firstGuidePoint;
            constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 left = width * glm::normalize(glm::cross(up, guideDirection));

            glm::vec3 midPoint = (*firstGuidePoint + *secondGuidePoint) / 2.0f;
            glm::vec3 normal = glm::normalize(glm::cross(guideDirection, left));

            size_t leftIndex = builder.pushVertex(midPoint + left, normal);
            size_t rightIndex = builder.pushVertex(midPoint - left, normal);
            if (!first) {
                builder.emitTriangle(previousLeftIndex, rightIndex, leftIndex);
                builder.emitTriangle(previousLeftIndex, previousRightIndex, rightIndex);
            }

            previousLeftIndex = leftIndex;
            previousRightIndex = rightIndex;
            firstGuidePoint = secondGuidePoint;
            secondGuidePoint++;

            first = false;
        }
        return builder.createMesh();
    }
}

TrackModel::TrackModel(ProgramFactory& factory, const NurbsCurve<glm::vec3>& curve, float width, size_t segments)
    : _program(factory.getProgram<DrawMaterialsProgram>())
    , _trackMesh(buildTrackMesh(curve, width, segments))
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