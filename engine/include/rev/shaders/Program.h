#pragma once

#include "rev/gl/ProgramResource.h"
#include "rev/shaders/Shader.h"

#include <fmt/format.h>

namespace rev {
class Program {
public:
    Program(const VertexShader& vShader, const FragmentShader& fShader);
    ProgramContext prepareContext() { return ProgramContext(_programResource); }

    template <typename ValueType>
    Uniform<ValueType> getUniform(std::string_view name)
    {
        auto iter = _uniforms.find(name);
        if (iter == _uniforms.end()) {
            throw std::runtime_error(fmt::format("Uniform \"{}\" not found.", name));
        }
        if (iter->second.type != std::type_index(typeid(ValueType))) {
            throw std::runtime_error(fmt::format("Uniform \"{}\" type mismatch.", name));
        }
        return _programResource.getUniform<ValueType>(name.data());
    }

private:
    std::unordered_map<size_t, IndexedShaderVariable> _inputs;
    std::unordered_map<size_t, IndexedShaderVariable> _outputs;
    std::unordered_map<std::string_view, ShaderVariable> _uniforms;

    ProgramResource _programResource;
};
}
