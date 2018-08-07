
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <Graphics/Graphics.h>
#include <Graphics/Layers/Layer2D.h>
#include <Utils/Clock.h>
#include <Utils/Log.h>
#include <Utils/FileUtils.h>
#include <filesystem>
#include "CubeData.h"


using namespace Engine;
using namespace Graphics;
using namespace Math;

float* generateHeightMap(int amount) {
	srand(time(NULL));
	float* height = new float[amount * amount]();
	for (int i = 0; i < amount; i++) {
		for (int j = 0; j < amount; j++) {
			float h = float(rand() % 10);
			height[i * amount + j] = h == 0 ? 1 : h;
		}
	}
	return height;
}




int main() {
	Utils::Log::getLog()->logInfo("Start");
	Window window = Window("Test", 720, 1280, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(glewInit() != GLEW_OK) {
		return EXIT_FAILURE;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Vec3 cubeColor = Vec3(0, 150, 0) / 255;
	Vec3 clearColor = Vec3(135, 206, 250) / 255;
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);

	

	const int amount = 20;
	float* height = generateHeightMap(amount);
	int blockAmount = std::pow(amount, 2);
	int max = blockAmount;
	for (int i = 0; i < max; i++)
		blockAmount += int(height[i]) - 1;


	std::vector<Mat4> translation;
	for (int x = 0; x < amount; x++) {
		for (int z = 0; z < amount; z++) {
			int h = int(height[x * amount + z]);
			for (int y = 0; y < h; y++)
				translation.push_back(Mat4::translation(Vec3(x, y, z)));
		}
	}

	delete[] height;

	VertexBuffer vbo = VertexBuffer(cubeVerticesUV, sizeof(cubeVerticesUV));
	VBLayout layout;
	layout.addElement(3, GL_FLOAT);
	layout.addElement(3, GL_FLOAT);
	layout.addElement(2, GL_FLOAT);
	VertexArray vao;
	vao.addBuffer(vbo, layout);
	VertexBuffer v(translation.data(), sizeof(Mat4) * translation.size());
	vao.addInstancedMatrixBuffer(v);
	std::string vertex = std::filesystem::absolute("Source/Resources/Shaders/CubeVertex.shader").generic_string();
	std::string frag = std::filesystem::absolute("Source/Resources/Shaders/CubeFragment.shader").generic_string();
	Shader shader(vertex, frag);
	Camera camera = Camera(window, 0.1f, 200.f);
	camera.setPosition(Vec3(0, 5, 0));
	shader.setUniform3f("u_Color", cubeColor);
	shader.setUniformMatrix4fv("u_View", camera.getViewMatrix());
	shader.setUniformMatrix4fv("u_Projection", camera.getProjectionMatrix());
	shader.setUniform1i("u_TextureSlot", 0);
	Texture texture("Source/Resources/Textures/grass_side.png", GL_NEAREST);
	texture.setSlot();
	BasicRenderer r;


	Utils::Clock clock;
	while (window.isKeyReleased(GLFW_KEY_ESCAPE) && !window.isClosed()) {
		r.renderArraysInstanced(vao, shader, 0, 36, translation.size());
		camera.update(window, clock.getTimePassed());
		clock.reset();
		shader.setUniformMatrix4fv("u_View", camera.getViewMatrix());
		window.update();
	}
	std::cout << "Hello" << std::endl;
	Utils::Log::getLog()->closeLog();
}