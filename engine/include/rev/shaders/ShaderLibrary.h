#pragma once

#include "rev/shaders/Program.h"
#include "rev/shaders/Shader.h"
#include <gsl/span>
#include <memory>

namespace rev {

class ShaderLibrary {
public:
    ShaderLibrary();
    ~ShaderLibrary();

    std::shared_ptr<Program> acquireProgram(gsl::span<const VertexShaderComponentInfo> vertexComponents,
        gsl::span<const FragmentShaderComponentInfo> fragmentComponents);

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

}
