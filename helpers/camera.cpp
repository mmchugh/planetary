#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "camera.h"

Camera::Camera()
{
    position = glm::vec3( 25, 0, 50 );
    horizontalAngle = 3.6f;
    verticalAngle = 0.0f;
    fieldOfView = 45.0f;

    updateVectors();
    updateView();
    updateProjection();
}

Camera::~Camera()
{
    // nothing to manually deallocate
}

void Camera::updateView()
{
    viewMatrix = glm::lookAt(position, position+direction, up);
}

void Camera::updateProjection()
{
    projectionMatrix = glm::perspective(fieldOfView, 4.0f / 3.0f, 0.1f, 10000.0f);
}

void Camera::updateVectors()
{
    direction = glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
    right = glm::vec3(sin(horizontalAngle - 3.14f/2.0f), 0, cos(horizontalAngle - 3.14f/2.0f));
    up = glm::cross(right, direction);
}

void Camera::advance(float speed)
{
    position += direction * speed;
    updateView();
}

void Camera::sideways(float speed)
{
    position += right * speed;
    updateView();
}

void Camera::rise(float speed)
{
    position += up * speed;
    updateView();
}

void Camera::yaw(float speed)
{
    verticalAngle += speed;

    if(verticalAngle > 3.14/2.0)
		verticalAngle = 3.14f/2.0f;
	if(verticalAngle < -3.14/2.0)
		verticalAngle = -3.14f/2.0f;

    updateVectors();
}

void Camera::pitch(float speed)
{
    horizontalAngle += speed;

    updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
    return projectionMatrix;
}
