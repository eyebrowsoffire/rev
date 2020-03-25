#pragma once

#include "rev/Utilities.h"
#include "rev/gl/ProgramResource.h"

#include <fmt/format.h>
#include <gsl/span>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace rev {

using VariableType = std::type_index;

struct ShaderVariable {
    VariableType type;
    std::string_view name;
};

struct IndexedShaderVariable : ShaderVariable {
    size_t index;
};

struct ShaderFunction {
    std::string_view name;
};

// struct ShaderComponent {
//     static gsl::span<ShaderVariable> inputs();
//     static gsl::span<ShaderVariable> outputs();
//     static gsl::span<ShaderVariable> uniforms();
//
//     static gsl::span<ShaderFunction> requiresFunctions();
//     static gsl::span<ShaderFunctions> fulfillsFunctions();
//
//     static std::string_view source();
// }

using ShaderComponentID = std::type_index;

struct VertexShaderTraits {
    using InputVariableType = IndexedShaderVariable;
    using OutputVariableType = ShaderVariable;
    using ShaderResourceType = VertexShaderResource;
};

struct FragmentShaderTraits {
    using InputVariableType = ShaderVariable;
    using OutputVariableType = IndexedShaderVariable;
    using ShaderResourceType = FragmentShaderResource;
};

template <typename Traits>
struct ShaderComponentInfo {
    ShaderComponentID componentID;

    gsl::span<typename Traits::InputVariableType> inputs;
    gsl::span<typename Traits::OutputVariableType> outputs;
    gsl::span<ShaderVariable> uniforms;

    gsl::span<ShaderFunction> requiresFunctions;
    gsl::span<ShaderFunction> fulfillsFunctions;

    std::string_view source;

    template <typename Component>
    static ShaderComponentInfo create()
    {
        return {
            std::type_index(typeid(Component)),
            Component::inputs(),
            Component::outputs(),
            Component::uniforms(),
            Component::requiresFunctions(),
            Component::fulfillsFunctions(),
            Component::source(),
        };
    }
};

template <typename Traits>
class Shader {
public:
    using InputVariableType = typename Traits::InputVariableType;
    using OutputVariableType = typename Traits::OutputVariableType;
    using ShaderResourceType = typename Traits::ShaderResourceType;

    Shader(gsl::span<const ShaderComponentInfo<Traits>> components);

    gsl::span<const InputVariableType> getInputs() const { return _inputs; }

    gsl::span<const OutputVariableType> getOutputs() const { return _outputs; }

    gsl::span<const ShaderVariable> getUniforms() const { return _uniforms; }

    const ShaderResourceType& getShaderResource() const { return _shaderResource; }

private:
    std::vector<InputVariableType> _inputs;
    std::vector<OutputVariableType> _outputs;
    std::vector<ShaderVariable> _uniforms;

    ShaderResourceType _shaderResource;
};

using VertexShader = Shader<VertexShaderTraits>;
using FragmentShader = Shader<FragmentShaderTraits>;

using VertexShaderComponentInfo = ShaderComponentInfo<VertexShaderTraits>;
using FragmentShaderComponentInfo = ShaderComponentInfo<FragmentShaderTraits>;

}