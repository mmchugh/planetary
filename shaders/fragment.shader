#version 330 core

// Interpolated values from the vertex shaders
in float intensity;

// Ouput data
out vec3 color;

void main(){

	color = intensity * vec3(1.0, 1.0, 1.0);
}
