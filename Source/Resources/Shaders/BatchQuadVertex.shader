#version 330 core

// Position 
layout(location = 0) in vec4 position;
// Texture coordinates
layout(location = 1) in vec2 uv;
// Which texture unit should this shader sample from
layout(location = 2) in int samplerID;
// Is this text (No bool in GLSL, so int is used)
layout(location = 3) in int isText;
// Text color, if this is text
layout(location = 4) in vec3 textColor;

// Variants for the fragment shader

out vec2 v_UV;
out vec3 v_TextColor;
flat out int v_IsText;
flat out int v_SamplerID;

// Uniforms

uniform mat4 projection;
uniform mat4 view;

void main() {
	mat4 mvp = projection * view;
	gl_Position = mvp * position;
	v_UV = uv;
	v_TextColor = textColor;
	v_IsText = isText;
	v_SamplerID = samplerID;
}