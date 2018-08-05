#version 430 core

out vec4 colour;

uniform sampler2D imTexture;

in vec2 UV;

void main(void) {   
	colour = texture(imTexture, UV);
}