#include "rev/MtlFile.h"

#include "rev/WavefrontHelpers.h"

#include <fstream>
#include <sstream>

namespace rev
{

MtlFile::MtlFile(const std::string &filePath)
{
    std::ifstream file{filePath};
    if (!file)
    {
        throw std::runtime_error("Unable to read MTL file.");
    }

    std::string currentMaterialName;
    MaterialProperties currentMaterialProperties;
    while (!file.eof())
    {
        std::string line;
        if (!std::getline(file, line))
        {
            break;
        }

        std::istringstream stream(line);
        std::string firstWord;
        stream >> firstWord;
        if (firstWord == "newmtl")
        {
            commitProperty(currentMaterialName, currentMaterialProperties);
            stream >> currentMaterialName;
            continue;
        }

        if (firstWord == "Ns")
        {
            stream >> currentMaterialProperties.specularExponent;
            continue;
        }

        if (firstWord == "Ka")
        {
            currentMaterialProperties.ambientColor = getVec3(stream);
            continue;
        }

        if (firstWord == "Kd")
        {
            currentMaterialProperties.diffuseColor = getVec3(stream);
            continue;
        }

        if (firstWord == "Ks")
        {
            currentMaterialProperties.specularColor = getVec3(stream);
            continue;
        }

        if (firstWord == "Ke")
        {
            currentMaterialProperties.emissiveColor = getVec3(stream);
        }
    }
    commitProperty(currentMaterialName, currentMaterialProperties);
}

const MaterialProperties *MtlFile::propertiesForMaterial(const std::string &materialName) const
{
    auto iter = _materialMap.find(materialName);
    return iter != _materialMap.end() ? &iter->second : nullptr;
}

void MtlFile::commitProperty(const std::string &name, const MaterialProperties &properties)
{
    if (name.empty())
    {
        return;
    }

    _materialMap[name] = properties;
}
} // namespace rev
