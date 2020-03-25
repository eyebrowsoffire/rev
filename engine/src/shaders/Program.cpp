#include "rev/shaders/Program.h"

namespace rev {

Program::Program(const VertexShader& vShader, const FragmentShader& fShader)
{
    for (const auto& input : fShader.getInputs()) {
        auto outputs = vShader.getOutputs();
        auto iter = std::find_if(outputs.begin(), outputs.end(),
            [name = input.name](const ShaderVariable& variable) { return variable.name == name; });
        if (iter == outputs.end()) {
            throw std::runtime_error(fmt::format(
                "Fragment shader input \"{}\" not found on vertex shader.", input.name));
        }
        if (iter->type != input.type) {
            throw std::runtime_error(fmt::format(
                "Type mismatch between vertex/fragment shader variable \"{}\"", input.name));
        }
    }

    for (const auto& input : vShader.getInputs()) {
        _inputs.insert_or_assign(input.index, input);
    }

    for (const auto& output : fShader.getOutputs()) {
        _outputs.insert_or_assign(output.index, output);
    }

    for (const auto& uniform : vShader.getUniforms()) {
        _uniforms.insert_or_assign(uniform.name, uniform);
    }

    for (const auto& uniform : fShader.getUniforms()) {
        auto iter = _uniforms.find(uniform.name);
        if (iter != _uniforms.end()) {
            if (iter->second.type != uniform.type) {
                throw std::runtime_error(
                    fmt::format("Type mismatch between vertex and fragment shader uniform \"{}\"",
                        uniform.name));
            }
        } else {
            _uniforms.insert_or_assign(uniform.name, uniform);
        }
    }

    _programResource.attachShader(vShader.getShaderResource());
    _programResource.attachShader(fShader.getShaderResource());
    _programResource.link();

    if (!_programResource.getLinkStatus()) {
        auto linkLog = _programResource.getLinkLog();
        throw std::runtime_error(fmt::format("Failed to link program. Link Log:\n{}", linkLog));
    }
}

}