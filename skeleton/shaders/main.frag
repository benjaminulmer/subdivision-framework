#version 430 core

out vec4 colour;

uniform vec3 objColour;

uniform bool fade;
uniform float maxDist;
uniform float minDist;

in vec3 N;
in vec3 L;
in vec3 V;

void main(void) {    	

	float diffuse =  (dot(N, L) + 1) / 2;

	// Calculate alpha if fading is needed
	if (fade) {
		float dist = length(V);

		// Fall off function
		float norm = (dist - minDist) / (maxDist - minDist);
		norm = 1.f - 2.5f * (norm - 0.f);

		if (norm < 0.f) norm = 0.f;
		if (norm > 1.f) norm = 1.f;

		colour = vec4(objColour, norm);
	}
	else {
		colour = vec4(objColour, 1.0);
	}
}