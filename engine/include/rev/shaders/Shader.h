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

    template <typename ValueType>
    static ShaderVariable make(std::string_view name)
    {
        return { std::type_index(typeid(ValueType)), name };
    }
};

struct IndexedShaderVariable : ShaderVariable {
    size_t index;

    template <typename ValueType>
    static IndexedShaderVariable make(std::string_view name, size_t index)
    {
        return IndexedShaderVariable{ std::type_index(typeid(ValueType)), name, index };
    }    
};

struct ShaderFunction {
    std::string_view name;
};

// struct ShaderComponent {
//     static inline const std::array<[Indexed]ShaderVariable> inputs;
//     static inline const std::array<[Indexed]ShaderVariable> outputs;
//     static inline const std::array<ShaderVariable> uniforms;
//
//     static constexpr std::array<ShaderFunction> requiresFunctions;
//     static constexpr std::array<ShaderFunctions> fulfillsFunctions;
//
//     static inline constexpr std::string_view source;
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

    gsl::span<const typename Traits::InputVariableType> inputs;
    gsl::span<const typename Traits::OutputVariableType> outputs;
    gsl::span<const ShaderVariable> uniforms;

    gsl::span<const ShaderFunction> requiresFunctions;
    gsl::span<const ShaderFunction> fulfillsFunctions;

    std::string_view source;

    template <typename Component>
    static constexpr ShaderComponentInfo make()
    {
        return {
            std::type_index(typeid(Component)),
            Component::inputs,
            Component::outputs,
            Component::uniforms,
            Component::requiresFunctions,
            Component::fulfillsFunctions,
            Component::source,
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