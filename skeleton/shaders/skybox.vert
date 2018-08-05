#version 430 core

uniform mat4 modelView;
uniform mat4 projection;
uniform float scale;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 colour;
layout (location = 2) in vec2 uv;

out vec2 UV;

void main(void) {	

	UV = uv;

	vec3 v = vertex;

	float s = (scale - 1.f) / 16.f;
	s = 1.f + s;

	// Code for billboarded skybox from http://www.lighthouse3d.com/opengl/billboarding/index.php?billCheat

	vec3 CameraRight_worldspace = {modelView[0][0], modelView[1][0], modelView[2][0]};
	vec3 CameraUp_worldspace = {modelView[0][1], modelView[1][1], modelView[2][1]};

	CameraRight_worldspace = normalize(CameraRight_worldspace);
	CameraUp_worldspace = normalize(CameraUp_worldspace);

	vec3 vertexPosition_worldspace = vec3(0.f, 0.f, 0.f) + (CameraRight_worldspace * v.x * s) + (CameraUp_worldspace * v.y * s);

	// Transform model and put in camera space
   // vec4 pCameraSpace = modelView * vec4(v, 1.0); 

   // gl_Position = projection * pCameraSpace;   

	gl_Position = projection * modelView * vec4(vertexPosition_worldspace, 1.f);
}
