#version 400 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 v_UV;
in vec3 v_FragmentPosition;

// The color of the ambient light in the scene, the scene's clear color
uniform vec3 u_GlobalLightColor;
// The color of the light originating from the cameras position
uniform vec3 u_LightColor;
// The position of the light (The light originating from the cameras position (player position))
uniform vec3 u_LightPosition;
// The intensity of the ambient light
uniform float u_AmbientIntensity;


uniform sampler2D u_TextureSlot;
uniform bool u_EnableLighting;

const float CONSTANT = 1.0f;
const float LINEAR = 0.014f;
const float QUADRATIC = 0.0007f;

const float b = cos(2);



void performLighting() {
	float distance = length(u_LightPosition - v_FragmentPosition);
	float attenuation = 1.0 / (CONSTANT + LINEAR * distance + QUADRATIC * (distance * distance));

	// Ambient lighting calculation
	vec3 ambient = u_AmbientIntensity * u_GlobalLightColor * 0.f;

	// Diffused lighting calculation
	vec3 norm = normalize(v_Normal);
	vec3 lightDirection = normalize(u_LightPosition - v_FragmentPosition);
	float diffuseLightingValue = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = diffuseLightingValue * u_LightColor;

	// Specular lighting calculation
	// No need to calculate the view direction since the lightdirection is already calculated with the cameras position
	vec3 reflectDir = reflect(-lightDirection, norm);
	float spec = pow(max(dot(lightDirection, reflectDir), 0.0), 32);
	vec3 specular = 0.5f * spec * u_LightColor;

	vec3 objColor = vec3(texture(u_TextureSlot, v_UV));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;


	color = vec4((ambient + diffuse + specular) * objColor, 1.0);
}

void main() {
	if (u_EnableLighting)
		performLighting();
	else
		color = texture(u_TextureSlot, v_UV);

	return;
}