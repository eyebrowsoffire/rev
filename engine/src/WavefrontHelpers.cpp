#include "rev/WavefrontHelpers.h"

#include "rev/CompositeModel.h"
#include "rev/MtlFile.h"
#include "rev/ObjFile.h"
#include "rev/SceneObjectGroup.h"
#include "rev/Utilities.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

namespace std {
template <>
struct hash<glm::uvec3> {
    size_t operator()(const glm::uvec3& s) const noexcept
    {
        size_t hash = 0;
        rev::hashCombine(hash, s.x);
        rev::hashCombine(hash, s.y);
        rev::hashCombine(hash, s.z);
        return hash;
    }
};
} // namespace std

namespace rev {

std::shared_ptr<SceneObjectGroup<CompositeModel>> createObjectGroupFromWavefrontFiles(
    ShaderLibrary& library, const ObjFile& objFile, const MtlFile& mtlFile)
{
    std::vector<VertexData> vertexAttributes;
    std::vector<GLuint> indices;
    std::vector<ModelComponent> components;
    std::unordered_map<glm::uvec3, GLuint> vertexMapping;

    size_t vertexOffset = 0;
    for (const auto& waveFrontObject : objFile.getWavefrontObjects()) {
        size_t indexOffset = indices.size();
        for (const auto& triangle : waveFrontObject.triangles) {
            for (const auto& vertex : triangle) {
                GLuint index;
                auto iter = vertexMapping.find(vertex);
                if (iter != vertexMapping.end()) {
                    index = iter->second;
                    Expects(iter->first == vertex);
                } else {
                    VertexData data;
                    data.position = objFile.positionAtIndex(vertex[0]);
                    data.normal = objFile.normalAtIndex(vertex[2]);

                    index = static_cast<GLuint>(vertexAttributes.size());
                    vertexAttributes.push_back(data);

                    vertexMapping[vertex] = index;
                }
                indices.push_back(index);
            }
        }

        const MaterialProperties* properties
            = mtlFile.propertiesForMaterial(waveFrontObject.materialName);
        Expects(properties != nullptr);

        components.emplace_back(
            static_cast<GLsizei>(indices.size() - indexOffset), indexOffset, *properties);
    }

    return std::make_shared<SceneObjectGroup<CompositeModel>>(library, std::move(components),
        gsl::span<const VertexData>(vertexAttributes), gsl::span<const GLuint>(indices));
}
} // namespace rev