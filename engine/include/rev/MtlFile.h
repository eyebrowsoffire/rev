#pragma once

#include "rev/MaterialProperties.h"
#include "rev/Types.h"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace rev {
class MtlFile {
public:
    MtlFile(const std::string& filePath);

    const MaterialProperties*
    propertiesForMaterial(const std::string& materialName) const;

private:
    void commitProperty(const std::string& name,
        const MaterialProperties& properties);
    std::unordered_map<std::string, MaterialProperties> _materialMap;
};
} // namespace rev