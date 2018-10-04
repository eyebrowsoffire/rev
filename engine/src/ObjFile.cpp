#include "rev/ObjFile.h"

#include "rev/IndexedModel.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>

namespace std
{
    template<> struct hash<glm::uvec3>
    {
        size_t operator()(const glm::uvec3& s) const noexcept
        {
            size_t x = std::hash<unsigned int>{}(s.x);
            size_t y = std::hash<unsigned int>{}(s.y);
            size_t z = std::hash<unsigned int>{}(s.z);

            return x ^ (y << 8) ^ (z << 16);
        }
    };
}

namespace rev {

ObjFile::ObjFile(const std::string &filePath) {
  std::ifstream file{filePath};
  if (!file) {
    throw std::runtime_error("Unable to read OBJ file.");
  }

  while (!file.eof()) {
      std::string line;
      if(!std::getline(file, line))
      {
          break;
      }

      processLine(line);
  }
}

void ObjFile::processLine(const std::string &line)
{
    if(line.empty() || line[0] == '#' || line[0] == '\r' || line[0] == '\n')
    {
        // Empty line or comment line
        return;
    }

    std::stringstream lineStream(line);
    std::string lineType;
    lineStream >> lineType;

    if(lineType == "v")
    {
        glm::vec3 position;
        lineStream >> position.x;
        lineStream >> position.y;
        lineStream >> position.z;
        _positions.push_back(position);
    }
    else if (lineType == "vt")
    {
        glm::vec2 uv;
        lineStream >> uv.x;
        lineStream >> uv.y;
        _textureCoordinates.push_back(uv);
    }
    else if (lineType == "vn")
    {
        glm::vec3 normal;
        lineStream >> normal.x;
        lineStream >> normal.y;
        lineStream >> normal.z;
        _normals.push_back(normal);
    }
    else if (lineType == "f")
    {
        std::vector<glm::uvec3> vertexIndexes;
        while (!lineStream.eof())
        {
            std::string vertexSpec;
            lineStream >> vertexSpec;

            if(vertexSpec.empty())
            {
                break;
            }
            glm::uvec3 vertex;
            int scannedCount = sscanf(vertexSpec.c_str(), "%u/%u/%u", &vertex[0], &vertex[1], &vertex[2]);
            if (scannedCount != 3)
            {
                throw std::runtime_error("Unexpected face vertex format in OBJ file.");
            }

            // Obj files are 1-indexed, so we need to subtract 1
            vertex -= glm::uvec3(1);

            vertexIndexes.push_back(vertex);
        }

        size_t vertexCount = vertexIndexes.size();
        if(vertexCount == 3)
        {
            // Just a triangle
            _triangles.push_back(IndexedTriangle{vertexIndexes[0], vertexIndexes[1], vertexIndexes[2]});
        }
        else if (vertexCount == 4)
        {
            // Quad
            _triangles.push_back(IndexedTriangle{vertexIndexes[0], vertexIndexes[1], vertexIndexes[2]});
            _triangles.push_back(IndexedTriangle{vertexIndexes[0], vertexIndexes[2], vertexIndexes[3]});
        }
    }
    else if (lineType == "g")
    {
        // Group. Just ignore this for now.
    }
    else
    {
        throw std::runtime_error("Unexpected data in OBJ file.");
    }
}

std::shared_ptr<IndexedModel> ObjFile::createIndexedModel() const 
{
    std::vector<VertexData> vertexAttributes;
    std::vector<GLuint> indexes;
    std::unordered_map<glm::uvec3, GLuint> vertexMapping;

    for(const auto& triangle : _triangles)
    {
        for(const auto& vertex : triangle)
        {
            GLuint index;
            auto iter = vertexMapping.find(vertex);
            if(iter != vertexMapping.end()) 
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