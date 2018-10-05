#pragma once

#include "rev/Camera.h"
#include "rev/Types.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/FrameBuffer.h"
#include "rev/gl/Program.h"
#include "rev/gl/Texture.h"
#include "rev/gl/Uniform.h"
#include "rev/gl/VertexArray.h"

#include <glm/glm.hpp>
#include <memory>

namespace rev
{
class Scene;

class SceneView
{
public:
  SceneView();
  void setScene(std::shared_ptr<Scene>);
  void setOutputSize(const RectSize<GLsizei> &outputSize);
  const RectSize<GLsizei> &getOutputSize() const;
  void render();

  const Texture &getOutputTexture() const;

  const std::shared_ptr<Camera> &getCamera() const;

private:
  std::shared_ptr<Scene> _scene;
  std::shared_ptr<Camera> _camera;

  RectSize<GLsizei> _outputSize;
  Program _geometryProgram;
  Program _lightingProgram;

  FrameBuffer _deferredFramebuffer;
  Texture _depthBuffer;
  Texture _sceneBaseColor;
  Texture _sceneNormals;
  Texture _scenePosition;

  FrameBuffer _outputFramebuffer;
  Texture _outputTexture;

  VertexArray _fullScreenVao;
  Buffer _fullScreenVertexBuffer;

  Uniform<glm::mat4> _model;
  Uniform<glm::mat4> _view;
  Uniform<glm::mat4> _projection;
  Uniform<glm::vec3> _faceBaseColor;

  Uniform<glm::vec3> _lightPosition;
  Uniform<glm::vec3> _lightBaseColor;
  Uniform<glm::vec3> _camPosition;
};
} // namespace rev