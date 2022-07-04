#include "Render.h"
#include "Game.h"
#include "Input.h"


void Render::Start() {
	gpu = kl::make<kl::gpu>(window);

	Render::window.resize = [&](const kl::uint2& newSize) {
		if (newSize.x > 0 && newSize.y > 0) {
			Render::gpu->regenInternal(newSize);
			Render::gpu->viewport({}, newSize);
			camera.aspect = float(newSize.x) / newSize.y;
		}
	};

	renderShaders = gpu->newShaders(kl::file::readString("source/Shaders/Render.hlsl"));

	gpu->bind(gpu->newSamplerState(true, true), 0);

	sphereMesh = gpu->newVertexBuffer("resource/meshes/sphere.obj");
	earthDayTexture = gpu->newShaderView(gpu->newTexture(
		kl::image(std::string("resource/textures/earth_day.png"))));
	earthNightTexture = gpu->newShaderView(gpu->newTexture(
		kl::image(std::string("resource/textures/earth_night.png"))));
	earthCloudsTexture = gpu->newShaderView(gpu->newTexture(
		kl::image(std::string("resource/textures/earth_clouds.png"))));
	earthNormalMap = gpu->newShaderView(gpu->newTexture(
		kl::image(std::string("resource/textures/earth_normal.png"))));
	earthRoughnessMap = gpu->newShaderView(gpu->newTexture(
		kl::image(std::string("resource/textures/earth_roughness.png"))));
	starsMilkyTexture = gpu->newShaderView(gpu->newTexture(
		kl::image(std::string("resource/textures/stars_milky.png"))));

	camera.position -= camera.forward() * 2.0f;

	timer.reset();
}

void Render::Update() {
	Input::Scroll();

	deltaT = (float)timer.interval();
	elapsedT = (float)timer.elapsed();

	gpu->clear(kl::colors::gray);

	gpu->bind(renderShaders);

	VS_CB vsData = {};
	vsData.wMatrix = kl::mat4::rotation(Game::sphereRotation);
	vsData.vpMatrix = camera.matrix();
	gpu->autoVertexCBuffer(vsData);

	PS_CB psData = {};
	psData.sunDirection = { Game::sunDirection.normalize(), 1.0f };
	psData.cameraPosition = { camera.position, 1.0f };
	gpu->autoPixelCBuffer(psData);

	gpu->bindPixelShaderView(earthDayTexture, 0);
	gpu->bindPixelShaderView(earthNightTexture, 1);
	gpu->bindPixelShaderView(earthCloudsTexture, 2);
	gpu->bindPixelShaderView(earthNormalMap, 3);
	gpu->bindPixelShaderView(earthRoughnessMap, 4);
	gpu->bindPixelShaderView(starsMilkyTexture, 5);

	gpu->draw(sphereMesh);

	gpu->swap(true);

	window.title(kl::format(int(1.0f / deltaT)));
}
