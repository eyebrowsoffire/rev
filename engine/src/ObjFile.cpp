#include "rev/ObjFile.h"

#include "rev/WavefrontHelpers.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace rev {

ObjFile::ObjFile(const std::string& filePath)
{
    std::ifstream file{ filePath };
    if (!file) {
        throw std::runtime_error("Unable to read OBJ file.");
    }

    bool firstObject = true;
    WavefrontObject object;
    while (!file.eof()) {
        std::string line;
        if (!std::getline(file, line)) {
            break;
        }

        std::istringstream lineStream(line);
        std::string lineType;
        lineStream >> lineType;

        if (lineType == "o") {
            if (!firstObject) {
                _wfObjects.push_back(std::move(object));
            } else {
                firstObject = false;
            }
            continue;
        }

        if (lineType == "v") {
            glm::vec3 position = getVec3(lineStream);
            _positions.push_back(position);
            continue;
        }

        if (lineType == "vt") {
            glm::vec2 uv = getVec2(lineStream);
            _textureCoordinates.push_back(uv);
            continue;
        }

        if (lineType == "vn") {
            glm::vec3 normal = getVec3(lineStream);
            _normals.push_back(normal);
            continue;
        }

        if (lineType == "f") {
            std::vector<glm::uvec3> vertexIndexes;
            while (!lineStream.eof()) {
                std::string vertexSpec;
                lineStream >> vertexSpec;

                if (vertexSpec.empty()) {
                    break;
                }
                glm::uvec3 vertex;
                int scannedCount = sscanf(vertexSpec.c_str(), "%u/%u/%u", &vertex[0],
                    &vertex[1], &vertex[2]);
                if (scannedCount != 3) {
                    throw std::runtime_error(
                        "Unexpected face vertex format in OBJ file.");
                }

                // Obj files are 1-indexed, so we need to subtract 1
                vertex -= glm::uvec3(1);

                vertexIndexes.push_back(vertex);
            }

            size_t vertexCount = vertexIndexes.size();
            if (vertexCount == 3) {
                // Just a triangle
                object.triangles.push_back(WavefrontObject::IndexedTriangle{
                    vertexIndexes[0], vertexIndexes[1], vertexIndexes[2] });
            } else if (vertexCount == 4) {
                // Quad
                object.triangles.push_back(WavefrontObject::IndexedTriangle{
                    vertexIndexes[0], vertexIndexes[1], vertexIndexes[2] });
                object.triangles.push_back(WavefrontObject::IndexedTriangle{
                    vertexIndexes[0], vertexIndexes[2], vertexIndexes[3] });
            }
        }

        if (lineType == "usemtl") {
            lineStream >> object.materialName;
        }
    }
    _wfObjects.push_back(object);
}

gsl::span<const ObjFile::WavefrontObject> ObjFile::getWavefrontObjects() const
{
    return _wfObjects;
}

const glm::vec3& ObjFile::positionAtIndex(size_t index) const
{
    return _positions[index];
}

const glm::vec2& ObjFile::textureCoordinateAtIndex(size_t index) const
{
    return _textureCoordinates[index];
}

const glm::vec3& ObjFile::normalAtIndex(size_t index) const
{
    return _normals[index];
}

} // namespace rev