#include "Render.h"
#include "Game.h"


struct VS_CB {
	kl::mat4 wMatrix;
	kl::mat4 vpMatrix;
};

struct PS_CB {
	kl::float4 sunDirection;
	kl::float4 cameraPosition;
	kl::float4 miscData;
};

void Render::Initialize() {
	depthState = Game::gpu->newDepthState(true, false, false);
	shaders = Game::gpu->newShaders(kl::file::readString("source/Shaders/Render.hlsl"));
	mesh = Game::gpu->newVertexBuffer("resource/meshes/sphere.obj");
	earthDayTexture = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_day.png"))));
	earthNightTexture = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_night.png"))));
	earthCloudsTexture = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_clouds.png"))));
	earthNormalMap = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_normal.png"))));
	earthRoughnessMap = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_roughness.png"))));
}

void Render::Update() {
	Game::gpu->bind(depthState);
	Game::gpu->bind(shaders);

	VS_CB vsData = {};
	vsData.wMatrix = kl::mat4::rotation(Game::sphereRotation);
	vsData.vpMatrix = Game::camera.matrix();
	Game::gpu->autoVertexCBuffer(vsData);

	PS_CB psData = {};
	psData.sunDirection = { Game::sunDirection.normalize(), 1.0f };
	psData.cameraPosition = { Game::camera.position, 1.0f };
	psData.miscData = {
		Game::elapsedT / 1800.0f,
		0.0f,
		0.0f,
		0.0f
	};
	Game::gpu->autoPixelCBuffer(psData);

	Game::gpu->bindPixelShaderView(earthDayTexture, 0);
	Game::gpu->bindPixelShaderView(earthNightTexture, 1);
	Game::gpu->bindPixelShaderView(earthCloudsTexture, 2);
	Game::gpu->bindPixelShaderView(earthNormalMap, 3);
	Game::gpu->bindPixelShaderView(earthRoughnessMap, 4);

	Game::gpu->draw(mesh);
}
