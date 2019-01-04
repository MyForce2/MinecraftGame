#version 400 core

// Position
layout(location = 0) in vec4 position;
// Normal direction to this vertex
layout(location = 1) in vec3 normal;
// Texture coordinates
layout(location = 2) in vec2 uv;
// Which textures should be sampled from the texture atlas
layout(location = 3) in vec3 texturePositions;
layout(location = 4) in mat4 model;

out vec3 v_Normal;
out vec2 v_UV;

uniform mat4 u_VP;
const float TEXTURE_SIZE = 1.0f / (256.0f / 16.0f);

void main() {
	mat4 mvp = u_VP * model;
	gl_Position = mvp * position;
	v_Normal = normal;
	float texPos = 0.f;
	if (gl_VertexID < 24)
		// Cube sides
		texPos = texturePositions.x;
	else if (gl_VertexID < 30)
		// Cube bottom
		texPos = texturePositions.y;
	else
		// Cube top
		texPos = texturePositions.z;
	v_UV = vec2(uv.x + TEXTURE_SIZE * texPos, uv.y);
}
