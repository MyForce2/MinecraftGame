#version 400 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
//layout(location = 3) in vec3 texturePositions;
layout(location = 3) in mat4 model;

out vec3 v_Normal;
out vec2 v_UV;

uniform mat4 u_VP;
const float TEXTURE_SIZE = 1.0f / (256.0f / 16.0f);

void main() {
	mat4 mvp = u_VP * model;
	gl_Position = mvp * position;
	v_Normal = normal;
	float texPos = 0.f;
	if (gl_VertexID < 24) {
		texPos = 1.0f; // texturePositions.x;
	} else if (gl_VertexID < 30) {
		texPos = 2.0f; // texturePositions.y;
	} else {
		texPos = 0.0f; // texturePositions.z;
	}
	v_UV = vec2(uv.x + TEXTURE_SIZE * texPos, uv.y);
}
