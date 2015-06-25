#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// Output data ; will be interpolated for each fragment.
out float intensity;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(position,1);

	vec4 normal_in_cameraspace = normalize((V * M * vec4(normal,0)));
	vec4 light_dir_in_cameraspace = normalize(V * vec4(1, 1, 0, 0));

	intensity = max(dot(normal_in_cameraspace, light_dir_in_cameraspace), 0.0);
}
