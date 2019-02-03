
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <Graphics/Graphics.h>
#include <Graphics/Layers/Layer2D.h>
#include <Utils//Clock.h>
#include <Utils/Log.h>
#include <Utils/FileUtils.h>
#include <filesystem>
#include <noise/noise.h>
#include <noiseutils.h>
#include <unordered_map>
#include "World//CubeData.h"
#include "World//Chunk.h"
#include "Graphics/Font/FontManager.h"
#include <boost/functional/hash.hpp>
#include "World//ChunkManager.h"
#include <string>


using namespace Engine;
using namespace Graphics;
using namespace Math;
using namespace Utils;
using namespace Minecraft;
using namespace World;



int main() {
	
	Utils::Log::getLog()->logInfo("Start");
	Window window = Window("Test", 720, 1080, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(glewInit() != GLEW_OK) {
		return EXIT_FAILURE;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	

	Vec3 cubeColor = Vec3(100, 150, 0) / 255;
	auto res = std::hash<Vec3>()(cubeColor);
	Vec3 clearColor = Vec3(135, 206, 250) / 255;
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);


	std::string v1("Source//Resources//Shaders//CubeVertex.shader");
	std::string f1("Source//Resources//Shaders//CubeFragment.shader");
	std::string v2("Source//Resources//Shaders//BatchQuadVertex.shader");
	std::string f2("Source//Resources//Shaders//BatchQuadFragment.shader");


	Camera camera = Camera(window, 0.1f, 500.f);
	camera.setPosition(Vec3(100.0f, 40.0f, 100.0f));
	camera.setPosition(Vec3(0.f));

	




	Mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
	Shader shader(v1, f1);
	shader.bind();

	shader.setUniformMatrix4fv("u_VP", vp);
	shader.setInt("u_TextureSlot", 0);
	Texture texture("Source//Resources/Textures/DefaultPack2.png", GL_NEAREST);
	texture.setSlot();
	BasicRenderer r;
	
	std::cout << glGetError() << std::endl;

	Utils::Clock clock;

	Layer2D l(600.f, 800.f, v2, f2);
	FontManager* manager = FontManager::getFontManager();
	const std::string fontPath = "Source//Resources/Fonts/arial.ttf";
	manager->add(fontPath, 48);
	auto f = manager->get(fontPath, 48);
	ChunkManager* cManager = ChunkManager::getManager();
	bool alt = false;
	if (alt) {
		std::thread worker(&ChunkManager::loadWorld, cManager);
		worker.join();
	} else {
		cManager->loadWorld();
	}
	cManager->initWorldGLData();
	//for (int i = 0; i < 2; i++) {
	//	for (int j = 0; j < 2; j++) {
	//		cManager->addChunk(IVec2(i, j));
	//	}
	//}
	cManager->setShader(&shader);
	
	Label lbl("Hello", f);
	lbl.setLabelColor(Vec3(255, 0, 0));
	lbl.setStartPosition(Vec2(10, 560));
	Utils::Clock fps;
	int frames = 0;
	std::vector<int> frameCount;

	while (window.isKeyReleased(GLFW_KEY_ESCAPE) && !window.isClosed()) {
		l.startFrame();
		l.add(lbl);
		texture.setSlot();
		cManager->updateRenderList(camera);
		cManager->render();
		camera.update(window, clock.getTimePassed());
		cManager->setPlayerPosition(camera.getPosition());
		clock.reset();
		vp = camera.getProjectionMatrix() * camera.getViewMatrix();
		shader.setUniformMatrix4fv("u_VP", vp);
		glEnable(GL_BLEND);
		l.render();
		glDisable(GL_BLEND);
		window.update();
		if (fps.getTimePassed() >= 1.0f) {
			lbl.setText(std::to_string(frames));
			fps.reset();
			frames = 0;
		}
		frames++;
	}
	std::cout << "Hello" << std::endl;



	Log::getLog()->logInfo("End");
	cManager->deleteManager();
	manager->deleteManager();
	Utils::Log::getLog()->closeLog();
}