#include "rev/shaders/Shader.h"

#include <fmt/format.h>

namespace rev {
namespace {
    template <typename Traits>
    struct ShaderBuilder {
        ShaderBuilder() { _sources.push_back(kShaderPreamble); }

        void addComponent(const ShaderComponentInfo<Traits>& info)
        {
            for (const auto& function : info.requiresFunctions) {
                auto iter = _functions.find(function.name);
                if (iter == _functions.end()) {
                    throw std::runtime_error(
                        fmt::format("Missing function required by component: {}.", function.name));
                }
            }

            for (const auto& function : info.fulfillsFunctions) {
                addObjectName(function.name);
                _functions[function.name] = function;
            }

            for (const auto& input : info.inputs) {
                addVariable(input, inputs);
            }
            for (const auto& output : info.outputs) {
                addVariable(output, outputs);
            }
            for (const auto& uniform : info.uniforms) {
                addVariable(uniform, uniforms);
            }

            _sources.push_back(info.source);
        }

        void build()
        {
            if(_functions.find("main") == _functions.end())
            {
                throw std::runtime_error("No main function found when building shader.");
            }

            shaderResource.setSource(_sources);
            shaderResource.compile();
            if (!shaderResource.getCompileStatus()) {
                auto compileLog = shaderResource.getCompileLog();
                throw std::runtime_error(fmt::format("Failed to compile shader. Compile Log:\n{}", compileLog));
            }
        }

        typename Traits::ShaderResourceType shaderResource;
        std::vector<ShaderComponentID> components;
        std::vector<typename Traits::InputVariableType> inputs;
        std::vector<typename Traits::OutputVariableType> outputs;
        std::vector<ShaderVariable> uniforms;

    private:
        static constexpr std::string_view kShaderPreamble = "#version 330 core\n";
        std::vector<std::string_view> _sources;
        std::unordered_map<std::string_view, ShaderFunction> _functions;
        std::unordered_set<std::string_view> _objectNames;

        void addObjectName(std::string_view objectName)
        {
            if (!_objectNames.insert(objectName).second) {
                throw std::runtime_error(fmt::format("Name collision: {}", objectName));
            }
        }

        void addVariable(const ShaderVariable& variable, std::vector<ShaderVariable>& variables)
        {
            addObjectName(variable.name);
            variables.push_back(variable);
        }

        void addVariable(
            const IndexedShaderVariable& variable, std::vector<IndexedShaderVariable>& variables)
        {
            auto iter = std::find_if(variables.begin(), variables.end(),
                [index = variable.index](
                    const IndexedShaderVariable& existing) { return existing.index == index; });
            if (iter != variables.end()) {
                throw std::runtime_error(
                    fmt::format("Layout index collision: {} and {} both specify index {}",
                        variable.name, iter->name, variable.index));
            }
            addObjectName(variable.name);
            variables.push_back(variable);
        }
    };

}

template <typename Traits>
Shader<Traits>::Shader(gsl::span<const ShaderComponentInfo<Traits>> components)
{
    ShaderBuilder<Traits> builder;
    for (const auto& component : components) {
        builder.addComponent(component);
    }
    builder.build();

    _inputs = std::move(builder.inputs);
    _outputs = std::move(builder.outputs);
    _uniforms = std::move(builder.uniforms);
    _shaderResource = std::move(builder.shaderResource);
}

template class Shader<VertexShaderTraits>;
template class Shader<FragmentShaderTraits>;

}