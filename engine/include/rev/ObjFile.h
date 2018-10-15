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

    using IndexedTriangle = std::array<glm::uvec3, 3>;
    std::vector<IndexedTriangle> triangles;
  };

  gsl::span<const WavefrontObject> getWavefrontObjects() const;
  const glm::vec3 &positionAtIndex(size_t index) const;
  const glm::vec2 &textureCoordinateAtIndex(size_t index) const;
  const glm::vec3 &normalAtIndex(size_t index) const;

private:
  void processLine(const std::string &line);

  std::vector<WavefrontObject> _wfObjects;
  std::vector<glm::vec3> _positions;
  std::vector<glm::vec2> _textureCoordinates;
  std::vector<glm::vec3> _normals;
};
} // namespace rev