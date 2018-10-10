#include "rev/WavefrontHelpers.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

namespace std
{
template <>
struct hash<glm::uvec3>
{
    size_t operator()(const glm::uvec3 &s) const noexcept
    {
        size_t x = std::hash<unsigned int>{}(s.x);
        size_t y = std::hash<unsigned int>{}(s.y);
        size_t z = std::hash<unsigned int>{}(s.z);

        return x ^ (y << 8) ^ (z << 16);
    }
};
} // namespace std

namespace rev
{

std::shared_ptr<IModel> createModelFromWavefrontFile(const std::string &filePath)
{
    std::vector<VertexData> vertexAttributes;
    std::vector<GLuint> indexes;
    std::unordered_map<glm::uvec3, GLuint> vertexMapping;

    for (const auto &triangle : _triangles)
    {
        for (const auto &vertex : triangle)
        {
            GLuint index;
            auto iter = vertexMapping.find(vertex);
            if (iter != vertexMapping.end())
            {
                index = iter->second;
                Expects(iter->first == vertex);
            }
            else
            {
                VertexData data;
                data.position = _positions[vertex[0]];
                data.textureCoordinate = _textureCoordinates[vertex[1]];
                data.normal = _normals[vertex[2]];

                index = vertexAttributes.size();
                vertexAttributes.push_back(data);

                vertexMapping[vertex] = index;
            }
            indexes.push_back(index);
        }
    }

    return std::make_shared<IndexedModel>(vertexAttributes, indexes);
}
} // namespace rev