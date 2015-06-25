#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "glm/glm.hpp"

class Camera {
private:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;


    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;

    float horizontalAngle;
    float verticalAngle;
    float fieldOfView;

    void updateVectors();
    void updateView();
    void updateProjection();

public:
    Camera();
    ~Camera();
    glm::vec3 position;
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    void advance(float speed);
    void sideways(float speed);
    void rise(float speed);

    void yaw(float speed);
    void pitch(float speed);

};


#endif
