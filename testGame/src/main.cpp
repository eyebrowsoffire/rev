#include <rev/Camera.h>
#include <rev/Engine.h>
#include <rev/Model.h>
#include <rev/Scene.h>
#include <rev/SceneView.h>
#include <rev/Window.h>

#include <glm/glm.hpp>

#include <chrono>
#include <cmath>

namespace
{
constexpr glm::vec3 kCubeVertices[] = {
    // Front face
    {-1.0, -1.0, 1.0},
    {1.0, 1.0, 1.0},
    {-1.0, 1.0, 1.0},
    {-1.0, -1.0, 1.0},
    {1.0, -1.0, 1.0},
    {1.0, 1.0, 1.0},

    // Back face
    {-1.0, -1.0, -1.0},
    {-1.0, 1.0, -1.0},
    {1.0, 1.0, -1.0},
    {-1.0, -1.0, -1.0},
    {1.0, 1.0, -1.0},
    {1.0, -1.0, -1.0},

    // Top face
    {-1.0, 1.0, 1.0},
    {1.0, 1.0, -1.0},
    {-1.0, 1.0, -1.0},
    {-1.0, 1.0, 1.0},
    {1.0, 1.0, 1.0},
    {1.0, 1.0, -1.0},

    // Bottom face
    {-1.0, -1.0, 1.0},
    {-1.0, -1.0, -1.0},
    {1.0, -1.0, -1.0},
    {-1.0, -1.0, 1.0},
    {1.0, -1.0, -1.0},
    {1.0, -1.0, 1.0},

    // Left face
    {-1.0, -1.0, -1.0},
    {-1.0, 1.0, 1.0},
    {-1.0, 1.0, -1.0},
    {-1.0, -1.0, -1.0},
    {-1.0, -1.0, 1.0},
    {-1.0, 1.0, 1.0},

    // Right face
    {1.0, -1.0, -1.0},
    {1.0, 1.0, -1.0},
    {1.0, 1.0, 1.0},
    {1.0, -1.0, -1.0},
    {1.0, 1.0, 1.0},
    {1.0, -1.0, 1.0}};

std::vector<glm::vec3> buildFlatNormalsForVertices(gsl::span<const glm::vec3> vertices) 
{
  Expects(vertices.size() % 3 == 0);
  size_t primitiveCount = vertices.size() / 3;
  std::vector<glm::vec3> normals;
  for(size_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++)
  {
    size_t vertexIndex = primitiveIndex * 3;
    glm::vec3 edge1 = vertices[vertexIndex] - vertices[vertexIndex + 1];
    glm::vec3 edge2 = vertices[vertexIndex + 2] - vertices[vertexIndex + 1];
    glm::vec3 normal = glm::cross(edge1, edge2);

    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);
  }

  return normals;
}

template <typename DurationType>
void updateCamera(rev::Camera &camera, DurationType elapsedTime)
{
  using FloatSeconds = std::chrono::duration<float>;

  constexpr float radius = 5.0f;
  float t = FloatSeconds(elapsedTime).count();
  float y = 3.0f * sin(t / 4.0f);
  float x = radius * cos(t);
  float z = radius * sin(t);

  camera.setPosition({x, y, z});
}
} // namespace

int main(void)
{
  rev::Engine engine;
  auto window = engine.createWindow("Test Game", {640, 480});
  window->makeCurrent();

  auto sceneView = engine.createSceneView();
  auto scene = engine.createScene();
  sceneView->setScene(scene);
  window->setSceneView(sceneView);

  auto verticesSpan = gsl::span<const glm::vec3>(kCubeVertices);
  auto normals = buildFlatNormalsForVertices(verticesSpan);

  auto cubeModel = std::make_shared<rev::Model>(verticesSpan, gsl::span<const glm::vec3>(normals));
  auto cubeObject = scene->addObject(cubeModel);

  auto camera = sceneView->getCamera();
  camera->setTarget({0.0, 0.0, 0.0});
  auto start = std::chrono::steady_clock::now();
  while (1)
  {
    auto elapsedTime = std::chrono::steady_clock::now() - start;
    updateCamera(*camera, elapsedTime);

    engine.update();
  }
  return 0;
}
