#version 430 core

uniform mat4 modelView;
uniform mat4 projection;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 N;
out vec2 UV;

void main(void) {	

	N = normal;
	UV = uv;

	// Transform model and put in camera space
    vec4 pCameraSpace = modelView * vec4(vertex, 1.0); 
	
    gl_Position = projection * pCameraSpace;   
}
