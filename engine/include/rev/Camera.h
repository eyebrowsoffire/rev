#pragma once

#include <glm/glm.hpp>

namespace rev
{

class Camera
{
  public:
    Camera();

    void setPosition(const glm::vec3 &position);
    const glm::vec3 &getPosition() const;
    void setTarget(const glm::vec3 &target);
    const glm::vec3 &getTarget() const;
    void setUpVector(const glm::vec3 &target);

    void setNearClip(float near);
    void setFarClip(float far);
    void setFieldOfView(float fov);
    void setAspectRatio(float aspect);

    const glm::mat4 &getViewMatrix();
    const glm::mat4 &getProjectionMatrix();

  private:
    void refreshViewMatrix();
    void refreshProjectionMatrix();

    glm::vec3 _position;
    glm::vec3 _target;
    glm::vec3 _upVector;

    glm::mat4 _viewMatrix;
    glm::mat4 _projectionMatrix;

    float _nearClip;
    float _farClip;
    float _fov;
    float _aspect;
    bool _viewDirty;
    bool _projectionDirty;
};

} // namespace rev