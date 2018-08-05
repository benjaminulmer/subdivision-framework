#version 430 core

uniform mat4 modelView;
uniform mat4 projection;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 colour;
layout (location = 2) in vec2 uv;

out vec3 C;
out vec3 L;
out vec3 V;
out vec2 UV;

void main(void) {	

	C = colour;
	UV = uv;

	vec3 lightPos = vec3(0.0, 10.0, 0.0);


	// Put light in camera space
	vec4 lightCameraSpace = modelView * vec4(lightPos, 1.0);
	

	// Transform model and put in camera space
    vec4 pCameraSpace = modelView * vec4(vertex, 1.0); 
	vec3 P = pCameraSpace.xyz;
	
	// Calculate L and V vectors
	L = normalize(lightCameraSpace.xyz - P);
	V = -P;

    gl_Position = projection * pCameraSpace;   
}
