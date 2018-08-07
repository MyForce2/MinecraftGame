#version 400 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 model;

out vec3 v_Normal;
out vec2 v_UV;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
	mat4 mvp = u_Projection * u_View * model;
	gl_Position = mvp * position;
	v_Normal = normal;
	v_UV = uv;
}
