
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
#include <unordered_map>
#include "World/CubeData.h"


using namespace Engine;
using namespace Graphics;
using namespace Math;
using namespace Utils;






void createChunk(std::vector<Mat4>& translation, const int amount) {
	noise::module::Perlin perlinModule;
	perlinModule.SetOctaveCount(1);
	perlinModule.SetFrequency(0.40);
	perlinModule.SetPersistence(2.0);
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(perlinModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(200, 200);
	heightMapBuilder.SetBounds(0.0, 4.0, 0.0, 4.0);
	heightMapBuilder.Build();

	int maxHeight = 30;

	Clock clock;

	auto getHeight = [&maxHeight](float heightValue) -> int {
		heightValue += 1;
		heightValue /= 2;
		int height = heightValue * maxHeight;
		return height <= 0 ? 1 : height;
	};
	
	for (int x = 0; x < amount; x++) {
		int height = getHeight(heightMap.GetValue(x, 0));
		int heightTwo = getHeight(heightMap.GetValue(x, amount - 1));
		for (int y = 0; y < height + 78; y++)
			translation.push_back(Mat4::translation(Vec3(x, y, 0)));
		for (int y = 0; y < heightTwo + 78; y++)
			translation.push_back(Mat4::translation(Vec3(x, y, amount - 1)));
	}

	for (int z = 0; z < amount; z++) {
		int height = getHeight(heightMap.GetValue(0, z));
		int heightTwo = getHeight(heightMap.GetValue(amount - 1, z));
		for (int y = 0; y < height + 78; y++)
			translation.push_back(Mat4::translation(Vec3(0, y, z)));
		for (int y = 0; y < heightTwo + 78; y++)
			translation.push_back(Mat4::translation(Vec3(amount - 1, y, z)));
	}

	for (int x = 0; x < amount; x++) {
		for (int z = 0; z < amount; z++) {
			int height = getHeight(heightMap.GetValue(x, z));
			translation.push_back(Mat4::translation(Vec3(x, height + 78, z)));
			translation.push_back(Mat4::translation(Vec3(x, 0, z)));
		}
	}


	std::cout << "Time : " << clock.getTimePassed() << "s\n";
}



int main() {
	
	Utils::Log::getLog()->logInfo("Start");
	Window window = Window("Test", 720, 1080, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(glewInit() != GLEW_OK) {
		return EXIT_FAILURE;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	

	Vec3 cubeColor = Vec3(100, 150, 0) / 255;
	Vec3 clearColor = Vec3(135, 206, 250) / 255;
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);

	const int amount = 200;
	int maxHeight = 30;
	std::vector<Mat4> translation;
	createChunk(translation, amount);

	int cubeAmount = std::pow(std::sqrt(translation.size()), 3);

	Utils::Log::getLog()->logInfo("Cube amount : " + std::to_string(cubeAmount));

	// Back + Front + Left + Right = 24 Vertices
	// Bottom = 6 Vertices
	// Top = 6 Vertices



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
	std::string v1 = "Source/Resources/Shaders/CubeVertex.shader";
	std::string f1 = "Source/Resources/Shaders/CubeFragment.shader";
	Camera camera = Camera(window, 0.1f, 500.f);
	camera.setPosition(Vec3(0, 130, 0));

	Mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
	Shader shader(v1, f1);
	shader.bind();

	shader.setUniformMatrix4fv("u_VP", vp);
	shader.setInt("u_TextureSlot", 0);
	Texture texture("Source/Resources/Textures/DefaultPack2.png", GL_NEAREST);
	texture.setSlot();
	BasicRenderer r;
	

	Utils::Clock clock;


	while (window.isKeyReleased(GLFW_KEY_ESCAPE) && !window.isClosed()) {
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