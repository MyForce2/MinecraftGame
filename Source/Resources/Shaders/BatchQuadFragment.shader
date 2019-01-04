#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_UV;
in vec3 v_TextColor;
// No interpolation used since these are ints (flat keyword)
flat in int v_IsText;
flat in int v_SamplerID;

const int TEXTURE_UNITS = 10;

uniform sampler2D u_TexSlots[TEXTURE_UNITS];

void main() {
	vec4 tex = texture(u_TexSlots[v_SamplerID], v_UV);
	if(v_IsText == 1)
		tex = vec4(v_TextColor, tex.a);
	color = tex;
}