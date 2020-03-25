#include "rev/shaders/ShaderLibrary.h"

namespace rev {
namespace {
    template <typename ShaderTraits>
    std::vector<ShaderComponentID> getComponentIDs(
        gsl::span<const ShaderComponentInfo<ShaderTraits>> components)
    {
        std::vector<ShaderComponentID> ids;
        for (const auto& component : components) {
            ids.push_back(component.componentID);
        }
        return ids;
    }
}
class ShaderLibrary::Impl {
public:
    std::shared_ptr<Program> acquireProgram(
        gsl::span<const VertexShaderComponentInfo> vertexComponents,
        gsl::span<const FragmentShaderComponentInfo> fragmentComponents)
    {
        ProgramKey programKey{
            getComponentIDs(vertexComponents),
            getComponentIDs(fragmentComponents),
        };
        std::shared_ptr<ProgramData> programData = _programs[programKey].lock();
        if (!programData) {
            auto vShader = acquireShader(vertexComponents, _vertexShaders);
            auto fShader = acquireShader(fragmentComponents, _fragmentShaders);
            programData = std::make_shared<ProgramData>(std::move(vShader), std::move(fShader));
            _programs[programKey] = programData;
        }

        // Aliased pointer
        return std::shared_ptr<Program>(std::move(programData), &(programData->getProgram()));
    }

private:
    struct ComponentsHasher {
        size_t operator()(const std::vector<ShaderComponentID>& components) const noexcept
        {
            size_t seed = 0;
            for (const auto& component : components) {
                hashCombine(seed, component);
            }
            return seed;
        }
    };

    struct ShaderKey {
        std::vector<ShaderComponentID> components;

        bool operator==(const ShaderKey& other) const noexcept
        {
            return components == other.components;
        }
    };

    struct ShaderKeyHasher {
        size_t operator()(const ShaderKey& key) const noexcept
        {
            return ComponentsHasher{}(key.components);
        }
    };

    struct ProgramKey {
        std::vector<ShaderComponentID> vectorComponents;
        std::vector<ShaderComponentID> fragmentComponents;

        bool operator==(const ProgramKey& other) const noexcept
        {
            return (vectorComponents == other.vectorComponents)
                && (fragmentComponents == other.fragmentComponents);
        }
    };

    struct ProgramKeyHasher {
        size_t operator()(const ProgramKey& key) const noexcept
        {
            size_t seed = 0;
            hashCombine<std::vector<ShaderComponentID>, ComponentsHasher>(
                seed, key.vectorComponents);
            hashCombine<std::vector<ShaderComponentID>, ComponentsHasher>(
                seed, key.fragmentComponents);
            return seed;
        }
    };

    class ProgramData {
    public:
        ProgramData(std::shared_ptr<VertexShader> vShader, std::shared_ptr<FragmentShader> fShader)
            : _program(*vShader, *fShader)
            , _vShader(std::move(vShader))
            , _fShader(std::move(fShader))
        {
        }

        Program& getProgram() { return _program; }

    private:
        Program _program;
        std::shared_ptr<VertexShader> _vShader;
        std::shared_ptr<FragmentShader> _fShader;
    };

    template <typename ShaderTraits>
    using ShaderMap
        = std::unordered_map<ShaderKey, std::weak_ptr<Shader<ShaderTraits>>, ShaderKeyHasher>;
    using ProgramMap = std::unordered_map<ProgramKey, std::weak_ptr<ProgramData>, ProgramKeyHasher>;

    template <typename ShaderTraits>
    std::shared_ptr<Shader<ShaderTraits>> acquireShader(
        gsl::span<const ShaderComponentInfo<ShaderTraits>> components,
        ShaderMap<ShaderTraits>& shaderMap)
    {
        auto shaderKey = ShaderKey{ getComponentIDs(components) };
        auto shader = shaderMap[shaderKey].lock();
        if (!shader) {
            shader = std::make_shared<Shader<ShaderTraits>>(components);
            shaderMap[shaderKey] = shader;
        }
        return shader;
    }

    ShaderMap<VertexShaderTraits> _vertexShaders;
    ShaderMap<FragmentShaderTraits> _fragmentShaders;
    ProgramMap _programs;
};

ShaderLibrary::ShaderLibrary()
    : _impl(std::make_unique<Impl>())
{
}

ShaderLibrary::~ShaderLibrary() = default;

std::shared_ptr<Program> ShaderLibrary::acquireProgram(
    gsl::span<const VertexShaderComponentInfo> vertexComponents,
    gsl::span<const FragmentShaderComponentInfo> fragmentComponents)
{
    return _impl->acquireProgram(vertexComponents, fragmentComponents);
}

}
