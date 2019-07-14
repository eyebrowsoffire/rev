#include "rev/SceneView.h"

#include "rev/Camera.h"
#include "rev/RenderStage.h"
#include "rev/Scene.h"

#include <iostream>

namespace rev {

SceneView::SceneView()
    : _camera(std::make_shared<Camera>())
{
}

void SceneView::setScene(std::shared_ptr<Scene> scene) { _scene = std::move(scene); }

void SceneView::setOutputSize(const RectSize<GLsizei>& outputSize)
{
    if (_outputSize.width == outputSize.width && _outputSize.height == outputSize.height) {
        return;
    }
    _outputSize = outputSize;
    _geometryStage.setOutputSize(outputSize);
    _lightingStage.setOutputSize(outputSize);
}

const RectSize<GLsizei>& SceneView::getOutputSize() const { return _outputSize; }

const std::shared_ptr<Camera>& SceneView::getCamera() const { return _camera; }

void SceneView::render()
{
    if (!_scene) {
        return;
    }

    // Geometry pass
    {
        auto fbContext = _geometryStage.getRenderContext();

        glViewport(0, 0, _outputSize.width, _outputSize.height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        _scene->renderAllObjects(*_camera);
    }

    // Lighting pass
    {
        auto fbContext = _lightingStage.getRenderContext();
        glViewport(0, 0, _outputSize.width, _outputSize.height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D, _geometryStage.getOutputTexture<WorldSpacePositionProperty>().getId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(
            GL_TEXTURE_2D, _geometryStage.getOutputTexture<WorldSpaceNormalProperty>().getId());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(
            GL_TEXTURE_2D, _geometryStage.getOutputTexture<AmbientMaterialProperty>().getId());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(
            GL_TEXTURE_2D, _geometryStage.getOutputTexture<EmissiveMaterialProperty>().getId());
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(
            GL_TEXTURE_2D, _geometryStage.getOutputTexture<DiffuseMaterialProperty>().getId());
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(
            GL_TEXTURE_2D, _geometryStage.getOutputTexture<SpecularMaterialProperty>().getId());
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(
            GL_TEXTURE_2D, _geometryStage.getOutputTexture<SpecularExponentProperty>().getId());

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        _scene->renderAllLights(*_camera);

        // Render debug overlays
        for (const auto& group : _debugGroups) {
            group->render(*_camera);
        }
    }
}

void SceneView::addDebugOverlayGroup(std::shared_ptr<ISceneObjectGroup> group)
{
    _debugGroups.push_back(group);
}

const Texture& SceneView::getOutputTexture() const
{
    return _lightingStage.getOutputTexture<OutputColorProperty>();
}

} // namespace rev