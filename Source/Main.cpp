
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <Graphics/Graphics.h>
#include <Graphics/Layers/Layer2D.h>
#include <Utils/Clock.h>
#include <Utils/Log.h>
#include <Utils/FileUtils.h>
#include <filesystem>
#include <noise/noise.h>
#include <noiseutils.h>
#include "CubeData.h"


using namespace Engine;
using namespace Graphics;
using namespace Math;






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

	const int amount = 100;

	noise::module::Perlin perlinModule;
	perlinModule.SetOctaveCount(1);
	perlinModule.SetFrequency(0.20);
	perlinModule.SetPersistence(1.0);
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(perlinModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(amount, amount);
	heightMapBuilder.SetBounds(0.0, 6.0, 0.0, 6.0);
	heightMapBuilder.Build();

	


	int maxHeight = 50;
	std::vector<float> ids;

	std::vector<Mat4> translation;
	for (int x = 0; x < amount; x++) {
		for (int z = 0; z < amount; z++) {
			float value = heightMap.GetValue(x, z) + 1;
			value /= 2;
			int h = value * maxHeight;
			/*int h = heightMap.GetValue(x, z) * maxHeight;*/
			h = h <= 0 ? 1 : h;
			for (int y = 0; y < h; y++) {
				translation.push_back(Mat4::translation(Vec3(x, y, z)));
				ids.push_back(rand() % 10);
			}
		}
	}
	Utils::Log::getLog()->logInfo("Cube amount : " + std::to_string(translation.size()));

	// Back + Front + Left + Right = 24 Vertices
	// Bottom = 6 Vertices
	// Top = 6 Vertices



	VertexBuffer vbo = VertexBuffer(cubeVerticesUV, sizeof(cubeVerticesUV));
	VertexBuffer k(ids.data(), ids.size() * sizeof(float));
	VBLayout layout;
	layout.addElement(3, GL_FLOAT);
	layout.addElement(3, GL_FLOAT);
	layout.addElement(2, GL_FLOAT);
	VBLayout c;
	c.addElement(1, GL_FLOAT);
	VertexArray vao;
	vao.addBuffer(vbo, layout);
	VertexBuffer v(translation.data(), sizeof(Mat4) * translation.size());
	vao.addInstancedBuffer(k, c);
	vao.addInstancedMatrixBuffer(v);
	
	std::string vertex = std::filesystem::absolute("Source/Resources/Shaders/CubeVertex.shader").generic_string();
	std::string frag = std::filesystem::absolute("Source/Resources/Shaders/CubeFragment.shader").generic_string();
	Shader shader(vertex, frag);
	Camera camera = Camera(window, 0.1f, 500.f);
	camera.setPosition(Vec3(0, 65, 0));
	shader.setUniform3f("u_Color", cubeColor);
	Mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
	shader.setUniformMatrix4fv("u_VP", vp);
	shader.setUniform1i("u_TextureSlot", 0);
	Texture texture("Source/Resources/Textures/DefaultPack2.png", GL_NEAREST);
	texture.setSlot();
	BasicRenderer r;
	

	Utils::Clock clock;


	while (window.isKeyReleased(GLFW_KEY_ESCAPE) && !window.isClosed()) {
		for(int i = 0; i < 1; i++)
			r.renderArraysInstanced(vao, shader, 0, 36, translation.size());
		camera.update(window, clock.getTimePassed());
		clock.reset();
		vp = camera.getProjectionMatrix() * camera.getViewMatrix();
		shader.setUniformMatrix4fv("u_VP", vp);
		window.update();
	}
	std::cout << "Hello" << std::endl;
	Utils::Log::getLog()->closeLog();
}