#version 330 core
#define NONE 0
#define INVERSION 1
#define GRAYSCALE 2
#define SHARPEN 3
#define BLUR 4
#define EDGE_DETECTION 5

layout(location = 0) out vec4 color;

in vec2 v_UV;

uniform sampler2D u_TextureSlot;
uniform int u_PPEffect;

const float OFFSET = 1.0 / 500.0;

float SHARPEN_KERNEL[9] = float[](
	-1, -1, -1,
	-1, 9, -1,
	-1, -1, -1
);

float BLUR_KERNEL[9] = float[]( 
	1.0 / 16, 2.0 / 16, 1.0 / 16,
	2.0 / 16, 4.0 / 16, 2.0 / 16,
	1.0 / 16, 2.0 / 16, 1.0 / 16
);

float EDGE_DETECTION_KERNEL[9] = float[](
	1, 1, 1,
	1, -8, 1,
	1, 1, 1
);

vec4 getSame(vec4 args) {
	return args;
}

vec3 getPPFragmentKernel(float kernel[9]) {
	vec2 offsets[9] = vec2[](
		vec2(-OFFSET, OFFSET), // top-left
		vec2(0.0f, OFFSET), // top-center
		vec2(OFFSET, OFFSET), // top-right
		vec2(-OFFSET, 0.0f),   // center-left
		vec2(0.0f, 0.0f),   // center-center
		vec2(OFFSET, 0.0f),   // center-right
		vec2(-OFFSET, -OFFSET), // bottom-left
		vec2(0.0f, -OFFSET), // bottom-center
		vec2(OFFSET, -OFFSET)  // bottom-right    
	);


	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(u_TextureSlot, v_UV + offsets[i]));
	}

	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];

	return col;
}

vec3 getPPFragment(int ppEffect) {
	vec3 texSample = vec3(texture(u_TextureSlot, v_UV));
	if (ppEffect == NONE) {
		return texSample;
	}
	if (ppEffect == INVERSION) {
		return 1.0 - texSample;
	}
	if (ppEffect == GRAYSCALE) {
		float average = 0.2126 * texSample.r + 0.7152 * texSample.g + 0.0722 * texSample.b;
		return vec3(average, average, average);
	}
	if (ppEffect == SHARPEN) {
		return getPPFragmentKernel(SHARPEN_KERNEL);
	}
	if (ppEffect == SHARPEN) {
		return getPPFragmentKernel(SHARPEN_KERNEL);
	}
	if (ppEffect == BLUR) {
		return getPPFragmentKernel(BLUR_KERNEL);
	}
	if (ppEffect == EDGE_DETECTION) {
		return getPPFragmentKernel(EDGE_DETECTION_KERNEL);
	}
}

void main() {
	color = vec4(getPPFragment(u_PPEffect), 1.0);
}