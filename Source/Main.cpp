
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
	std::string v3("Source//Resources//Shaders//PPVertex.shader");
	std::string f3("Source//Resources//Shaders//PPFragment.shader");


	Camera camera = Camera(window, 0.1f, 500.f);
	camera.setPosition(Vec3(100.0f, 40.0f, 100.0f));
	camera.setPosition(Vec3(0.f));

	

	GLfloat PP_QUAD_VERTICES[] = {
		1.0f,  1.0f, 1.0f, 1.0f,
	   -1.0f,  1.0f, 0.0f, 1.0f,
	   -1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
	};

	GLushort PP_QUAD_INDICES[] = {
		0, 1, 2,
		0, 2, 3
	};

	VertexBuffer ppVBO(PP_QUAD_VERTICES, sizeof(PP_QUAD_VERTICES), GL_STATIC_DRAW);
	IndexBuffer ppIBO(PP_QUAD_INDICES, 6);
	VBLayout ppLayout;
	VertexArray ppVAO;
	ppLayout.addElement(2, GL_FLOAT);
	ppLayout.addElement(2, GL_FLOAT);
	ppVAO.addBuffer(ppVBO, ppLayout);
	Shader ppShader(v3, f3);
	ppShader.bind();
	ppShader.setInt("u_TextureSlot", 0);
	ppShader.setInt("u_PPEffect", 0);
	
	Mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
	Shader shader(v1, f1);
	shader.bind();
	shader.setUniformMatrix4fv("u_VP", vp);
	shader.setInt("u_TextureSlot", 1);
	shader.setInt("u_EnableLighting", 1);
	shader.setFloat("u_AmbientIntensity", 0.175f);
	shader.setVec3("u_GlobalLightColor", clearColor);
	shader.setVec3("u_LightColor", Vec3(1.0));
	shader.setVec3("u_LightPosition", camera.getPosition());
	Texture texture("Source//Resources/Textures/DefaultPack2.png", GL_NEAREST);
	texture.setSlot(1);
	BasicRenderer r;
	
	std::cout << glGetError() << std::endl;


	Utils::Clock clock;

	Layer2D l(600.f, 800.f, v2, f2);
	FontManager* manager = FontManager::getFontManager();
	const std::string fontPath = "Source//Resources/Fonts/arial.ttf";
	manager->add(fontPath, 48);
	auto f = manager->get(fontPath, 48);
	ChunkManager* cManager = ChunkManager::getManager();
	cManager->loadWorld();
	cManager->initWorldGLData();
	cManager->setShader(&shader);
	
	Label lbl("Hello", f);
	lbl.setLabelColor(Vec3(255, 0, 0));
	lbl.setStartPosition(Vec2(10, 560));
	Utils::Clock fps;
	int frames = 0;
	std::vector<int> frameCount;

	FrameBuffer fbo(window.getSize().x, window.getSize().y);
	fbo.bind();
	fbo.bindRenderBuffer();
	fbo.bindTexture();

	bool lighting = true;
	int ppEffect = 0;
	Utils::Clock keyCooldown;
	std::cout << glGetError() << std::endl;

	while (window.isKeyReleased(GLFW_KEY_ESCAPE) && !window.isClosed()) {
		l.startFrame();
		l.add(lbl);
		if (window.isKeyPressed(GLFW_KEY_1) && keyCooldown.getTimePassed() > 0.25f) {
			ppEffect = ppEffect == 5 ? 0 : ppEffect + 1;
			ppShader.bind();
			ppShader.setInt("u_PPEffect", ppEffect);
			shader.bind();
			keyCooldown.reset();
		}
		if (window.isKeyPressed(GLFW_KEY_2) && keyCooldown.getTimePassed() > 0.25f) {
			lighting = !lighting;
			shader.setInt("u_EnableLighting", lighting);
			keyCooldown.reset();
		}
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
		fbo.bind();
		fbo.bindTexture();
		FrameBuffer::clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		texture.setSlot(1);
		cManager->updateRenderList(camera);
		cManager->render();
		camera.update(window, clock.getTimePassed());
		cManager->setPlayerPosition(camera.getPosition());
		clock.reset();
		fbo.unBind();
		fbo.unBindTexture();
		glClearColor(1.0, 1.0, 1.0, 1.0);
		FrameBuffer::clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		r.renderElements(ppVAO, ppShader, ppIBO);
		vp = camera.getProjectionMatrix() * camera.getViewMatrix();
		shader.setUniformMatrix4fv("u_VP", vp);
		shader.setVec3("u_LightPosition", camera.getPosition());
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