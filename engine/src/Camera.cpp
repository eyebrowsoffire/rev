#include "rev/Camera.h"

#include <glm/ext.hpp>

namespace rev
{

Camera::Camera()
    : _position(0.0f), 
      _target(0.0, 0.0, -1.0), 
      _upVector(0.0, 1.0, 0.0),
      _nearClip(0.1f), 
      _farClip(100.0f), 
      _fov(glm::half_pi<float>()),
      _aspect(16.0 / 9.0),
      _viewDirty(true),
      _projectionDirty(true) {}

void Camera::setPosition(const glm::vec3 &position)
{
    _position = position;
    _viewDirty = true;
}

const glm::vec3 &Camera::getPosition() const
{
    return _position;
}

void Camera::setTarget(const glm::vec3 &target)
{
    _target = target;
    _viewDirty = true;
}

void Camera::setUpVector(const glm::vec3 &upVector)
{
    _upVector = upVector;
    _viewDirty = true;
}

void Camera::setNearClip(float near)
{
    _nearClip = near;
    _projectionDirty = true;
}

void Camera::setFarClip(float far) { _farClip = far; }

void Camera::setFieldOfView(float fov)
{
    _fov = fov;
    _projectionDirty = true;
}

void Camera::setAspectRatio(float aspect)
{
    _aspect = aspect;
    _projectionDirty = true;
}

const glm::mat4 &Camera::getViewMatrix()
{
    refreshViewMatrix();
    return _viewMatrix;
}

const glm::mat4 &Camera::getProjectionMatrix()
{
    refreshProjectionMatrix();
    return _projectionMatrix;
}

void Camera::refreshViewMatrix()
{
    if (_viewDirty)
    {
        _viewMatrix = glm::lookAt(_position, _target, _upVector);
        _viewDirty = false;
    }
}

void Camera::refreshProjectionMatrix()
{
    if (_projectionDirty)
    {
        _projectionMatrix = glm::perspective(_fov, _aspect, _nearClip, _farClip);
        _projectionDirty = false;
    }
}

} // namespace rev
