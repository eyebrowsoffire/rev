#include "rev/TrackModel.h"

#include "rev/MaterialProperties.h"
#include "rev/ProgramFactory.h"

#include "rev/Mesh.h"
#include <vector>

namespace rev {

namespace {
    constexpr MaterialProperties kMaterialProperties{ glm::vec3(0.763975f, 0.763975f, 0.763975f),
        glm::vec3(0.000000f, 0.000000f, 0.000000f), glm::vec3(0.114541f, 0.101377f, 0.288820f),
        glm::vec3(0.6f, 0.6f, 0.6f), 445.098039f };

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
            context.setupVertexAttribute<decltype(position)>(
                0, offsetof(TrackVertexData, position), sizeof(TrackVertexData));
            context.setupVertexAttribute<decltype(normal)>(
                1, offsetof(TrackVertexData, normal), sizeof(TrackVertexData));
        }
    };
}

class TrackBuilder {
public:
    TrackBuilder(const NurbsCurve<glm::vec3>& curve, float width, size_t segments)
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

        auto firstGuidePoint = guidePoints.begin();
        auto secondGuidePoint = firstGuidePoint + 1;
        auto guidePointsEnd = guidePoints.end();
        glm::vec3 previousLeftPosition;
        glm::vec3 previousRightPosition;
        size_t previousLeftIndex;
        size_t previousRightIndex;

        bool first = true;
        while (secondGuidePoint != guidePointsEnd) {
            glm::vec3 guideDirection = *secondGuidePoint - *firstGuidePoint;
            constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 left = width * glm::normalize(glm::cross(up, guideDirection));

            glm::vec3 midPoint = (*firstGuidePoint + *secondGuidePoint) / 2.0f;
            glm::vec3 normal = glm::normalize(glm::cross(guideDirection, left));

            glm::vec3 leftPosition = midPoint + left;
            glm::vec3 rightPosition = midPoint - left;
            size_t leftIndex = _meshBuilder.pushVertex(leftPosition, normal);
            size_t rightIndex = _meshBuilder.pushVertex(rightPosition, normal);
            if (!first) {
                _meshBuilder.emitTriangle(previousLeftIndex, rightIndex, leftIndex);
                _meshBuilder.emitTriangle(previousLeftIndex, previousRightIndex, rightIndex);

                _mapBuilder.addTriangle({ previousLeftPosition, rightPosition, leftPosition }, {});
                _mapBuilder.addTriangle(
                    { previousLeftPosition, previousRightPosition, rightPosition }, {});
            }

            previousLeftIndex = leftIndex;
            previousRightIndex = rightIndex;
            previousLeftPosition = leftPosition;
            previousRightPosition = rightPosition;
            firstGuidePoint = secondGuidePoint;
            secondGuidePoint++;

            first = false;
        }
    }

    Mesh buildMesh() { return _meshBuilder.createMesh(); }

    KDTree<TrackModel::SurfaceData> buildMap() { return _mapBuilder.build(); }

private:
    MeshBuilder<TrackVertexData> _meshBuilder;
    KDTreeBuilder<TrackModel::SurfaceData> _mapBuilder;
};

TrackModel::TrackModel(
    ProgramFactory& factory, const NurbsCurve<glm::vec3>& curve, float width, size_t segments)
    : TrackModel(factory, TrackBuilder(curve, width, segments))
{
}

TrackModel::TrackModel(ProgramFactory& factory, TrackBuilder&& builder)
    : _program(factory.getProgram<DrawMaterialsProgram>())
    , _trackMesh(builder.buildMesh())
    , _surfaceMap(builder.buildMap())
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