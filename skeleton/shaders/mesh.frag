#version 430 core

out vec4 colour;

uniform sampler2D image;
uniform int hasTexture;
uniform vec3 objColour;
in vec3 N;
in vec3 L;
in vec3 V;
in vec2 UV;

void main(void) {    	

	float diffuse =  (dot(N, L) + 1) / 2;

	// Use for shading
	//vec3 col = vec3(1.0, 1.0, 1.0);
	colour = vec4(objColour * diffuse, 1.0);

	// Use for flat shading
	//colour = vec4(0.0, 0.0, 0.0, 1.0);
}