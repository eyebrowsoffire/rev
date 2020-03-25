#pragma once

#include "rev/shaders/Shader.h"
#include "rev/gl/ProgramResource.h"

namespace rev {
class Program {
public:
    Program(const VertexShader& vShader, const FragmentShader& fShader);
    ProgramContext prepareContext() { return ProgramContext(_programResource); }

private:
    std::unordered_map<size_t, IndexedShaderVariable> _inputs;
    std::unordered_map<size_t, IndexedShaderVariable> _outputs;
    std::unordered_map<std::string_view, ShaderVariable> _uniforms;

    ProgramResource _programResource;
};
}
