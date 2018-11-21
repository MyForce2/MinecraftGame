#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_UV;
in vec3 v_TextColor;
flat in int v_IsText;
flat in int v_SamplerID;

uniform sampler2D u_TexSlots[10];

void main() {
	vec4 tex = texture(u_TexSlots[v_SamplerID], v_UV);
	tex = vec4(v_TextColor, tex.a);
	color = tex;
}