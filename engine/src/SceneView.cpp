#include "rev/SceneView.h"

#include "rev/Scene.h"

#include <OpenGL/gl3.h>

namespace rev
{
void SceneView::setScene(std::shared_ptr<Scene> scene)
{
    _scene = std::move(scene);
}

void SceneView::render()
{
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace rev