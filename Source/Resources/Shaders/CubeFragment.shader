#version 400 core

layout(location = 0) out vec4 color;

in vec2 v_Normal;
in vec2 v_UV;

uniform vec3 u_Color;
uniform sampler2D u_TextureSlot;

void main() {
	color = texture(u_TextureSlot, v_UV);
}