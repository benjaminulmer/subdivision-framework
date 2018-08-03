#version 430 core

out vec4 colour;

uniform sampler2D image;
uniform vec3 objColour;
in vec3 N;
in vec2 UV;

void main(void) {    	
	
	if(N.x < 0.05) {
		colour = vec4(texture(image, UV).rgb, 1.0);
	} else {
		colour = vec4(N, 1.0);
	}

	//colour = vec4(N, 1.0);

}