#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string_view>
#include <vector>

namespace rev 
{
    class IndexedModel;

    class ObjFile 
    {
    public:
        ObjFile(const std::string &path);

        std::shared_ptr<IndexedModel> createIndexedModel() const;
    private:
        void processLine(const std::string &line);

        std::vector<glm::vec3> _positions;
        std::vector<glm::vec2> _textureCoordinates;
        std::vector<glm::vec3> _normals;

        using IndexedTriangle = std::array<glm::uvec3, 3>;
        std::vector<IndexedTriangle> _triangles;
    };
}