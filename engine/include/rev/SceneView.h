#pragma once

#include "rev/Camera.h"
#include "rev/RenderStage.h"
#include "rev/SceneObjectGroup.h"
#include "rev/Types.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/FrameBuffer.h"
#include "rev/gl/ProgramResource.h"
#include "rev/gl/Texture.h"
#include "rev/gl/Uniform.h"
#include "rev/gl/VertexArray.h"

#include <glm/glm.hpp>
#include <memory>

namespace rev {
class Scene;

class SceneView {
public:
    SceneView();
    void setScene(std::shared_ptr<Scene>);
    void setOutputSize(const RectSize<GLsizei>& outputSize);
    const RectSize<GLsizei>& getOutputSize() const;
    void render();

    void addDebugOverlayGroup(std::shared_ptr<ISceneObjectGroup> group);

    const Texture& getOutputTexture() const;

    const std::shared_ptr<Camera>& getCamera() const;

private:
    std::shared_ptr<Scene> _scene;
    std::shared_ptr<Camera> _camera;

    RectSize<GLsizei> _outputSize;

    struct ViewSpaceNormalProperty {
    };
    struct ViewSpacePositionProperty {
    };
    struct AmbientMaterialProperty {
    };
    struct EmissiveMaterialProperty {
    };
    struct DiffuseMaterialProperty {
    };
    struct SpecularMaterialProperty {
    };
    struct SpecularExponentProperty {
    };
    struct DepthProperty {
    };

    using ViewSpacePositionAttachment = RenderStageAttachment<ViewSpacePositionProperty, GL_COLOR_ATTACHMENT0,
        GL_RGB16F, GL_RGB, GL_FLOAT>;
    using ViewSpaceNormalAttachment = RenderStageAttachment<ViewSpaceNormalProperty, GL_COLOR_ATTACHMENT1,
        GL_RGB16F, GL_RGB, GL_FLOAT>;
    using AmbientAttachment = RenderStageAttachment<AmbientMaterialProperty, GL_COLOR_ATTACHMENT2,
        GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE>;
    using EmissiveAttachment = RenderStageAttachment<EmissiveMaterialProperty, GL_COLOR_ATTACHMENT3,
        GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE>;
    using DiffuseAttachment = RenderStageAttachment<DiffuseMaterialProperty, GL_COLOR_ATTACHMENT4,
        GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE>;
    using SpecularAttachment = RenderStageAttachment<SpecularMaterialProperty, GL_COLOR_ATTACHMENT5,
        GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE>;
    using SpecularExponentAttachment = RenderStageAttachment<SpecularExponentProperty, GL_COLOR_ATTACHMENT6,
        GL_R16F, GL_RED, GL_UNSIGNED_BYTE>;
    using DepthAttachment = RenderStageAttachment<DepthProperty, GL_DEPTH_ATTACHMENT,
        GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
        GL_UNSIGNED_BYTE>;

    using GeometryStage = RenderStage<ViewSpacePositionAttachment, ViewSpaceNormalAttachment,
        AmbientAttachment, EmissiveAttachment, DiffuseAttachment,
        SpecularAttachment, SpecularExponentAttachment,
        DepthAttachment>;
    GeometryStage _geometryStage;

    struct OutputColorProperty {
    };
    using OutputColorAttachment = RenderStageAttachment<OutputColorProperty, GL_COLOR_ATTACHMENT0,
        GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE>;
    using LightingStage = RenderStage<OutputColorAttachment>;
    LightingStage _lightingStage;

    std::vector<std::shared_ptr<ISceneObjectGroup>> _debugGroups;
};
} // namespace rev