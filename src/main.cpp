#include "glew/glew.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "helpers/shader.h"
#include "helpers/camera.h"

#include "noise/noise.h"

#include "contouring/mesh.h"
#include "contouring/octree.h"

#define SPEED 0.5

float forwardSpeed = 0.0;
float sideSpeed = 0.0;
float verticalSpeed = 0.0;

bool rotating = false;

double last_mouse_x = 0.0;
double last_mouse_y = 0.0;

Camera* cam;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        forwardSpeed = 1.0;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        forwardSpeed = -1.0;

    if ((key == GLFW_KEY_W || key == GLFW_KEY_S) && action == GLFW_RELEASE)
        forwardSpeed = 0.0;

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        sideSpeed = -1.0;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        sideSpeed = 1.0;

    if ((key == GLFW_KEY_A || key == GLFW_KEY_D) && action == GLFW_RELEASE)
        sideSpeed = 0.0;

    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        verticalSpeed = 1.0;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        verticalSpeed = -1.0;

    if ((key == GLFW_KEY_Q || key == GLFW_KEY_E) && action == GLFW_RELEASE)
        verticalSpeed = 0.0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        rotating = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        rotating = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(!rotating) return;

    double deltaX = xpos - last_mouse_x;
    double deltaY = ypos - last_mouse_y;
    last_mouse_x = xpos;
    last_mouse_y = ypos;

    cam->yaw(deltaY / 180.0 * -1.0);
    cam->pitch(deltaX / 180.0 * -1.0);
}

void update()
{
    glm::vec3 position = cam->position;

    double distance = glm::distance(position, glm::vec3(0, 0, 0));

    double speed = SPEED * ((distance-100.0) / 400.0);
    if (speed < 0.05) speed = 0.05;
    static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();

	float elapsedTime = float(currentTime - lastTime);

	cam->advance(forwardSpeed * elapsedTime * speed);
	cam->sideways(sideSpeed * elapsedTime * speed);
	cam->rise(verticalSpeed * elapsedTime * speed);

}
int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( 1024, 768, "Planet", NULL, NULL);
    if(!window)
    {
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    fprintf (stdout, "Renderer: %s\n", renderer);
    fprintf (stdout, "OpenGL version supported %s\n", version);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders("vertex.shader", "fragment.shader");

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    Mesh mesh;
	mesh.initialise();

	OctreeNode* root = nullptr;
	const int octreeSize = 64;

	VertexBuffer vertices_dc;
	IndexBuffer indices_dc;

    double start = glfwGetTime();
    fprintf (stdout, "starting generation\n");
    root = BuildOctree(glm::ivec3(-octreeSize / 2), octreeSize, 0.1f);
    GenerateMeshFromOctree(root, vertices_dc, indices_dc);
    fprintf (stdout, "generating world took: %0.3f\n", glfwGetTime() - start);
    mesh.uploadData(vertices_dc, indices_dc);

	glUseProgram(programID);

	cam = new Camera();

	float rotation = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        update();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);


        glm::mat4 ProjectionMatrix = cam->getProjectionMatrix();
		glm::mat4 ViewMatrix = cam->getViewMatrix();
		glm::mat4 ModelMatrix = glm::rotate(glm::mat4(1.0), rotation, glm::vec3(0.0, 1.0, 0.0));
		rotation += 0.01;
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(mesh.vertexArrayObj_);
		glDrawElements(GL_TRIANGLES, mesh.numIndices_, GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    mesh.destroy();
	glDeleteProgram(programID);

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
