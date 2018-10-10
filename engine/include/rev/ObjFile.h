#pragma once

#include <glm/glm.hpp>
#include <gsl/span>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace rev
{
class IndexedModel;

class ObjFile
{
  public:
    ObjFile(const std::string &path);

    struct WavefrontObject
    {
        std::string materialName;
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> textureCoordinates;
        std::vector<glm::vec3> normals;

        using IndexedTriangle = std::array<glm::uvec3, 3>;
        std::vector<IndexedTriangle> triangles;
    };

    gsl::span<const WavefrontObject> getWavefrontObjects() const;

  private:
    void processLine(const std::string &line);

    std::vector<WavefrontObject> _wfObjects;

};
} // namespace rev