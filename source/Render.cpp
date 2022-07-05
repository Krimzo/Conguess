#include "Render.h"
#include "Game.h"
#include "Input.h"
#include "Skybox.h"


kl::reference<Render::Skybox> skybox;

void Render::Start() {
	gpu = kl::make<kl::gpu>(window);
	gpu->bind(gpu->newSamplerState(true, false), 0);

	defaultDepth = gpu->newDepthState(true, false, false);
	skyboxDepth = gpu->newDepthState(false, false, false);

	Render::window.resize = [&](const kl::uint2& newSize) {
		if (newSize.x > 0 && newSize.y > 0) {
			Render::gpu->regenInternal(newSize);
			Render::gpu->viewport({}, newSize);
			camera.aspect = float(newSize.x) / newSize.y;
		}
	};
	Render::window.keys.f11.press = [&]() {
		static bool previousState = false;
		previousState = !previousState;
		Render::window.fullscreen(previousState);
	};

	renderShaders = gpu->newShaders(kl::file::readString("source/Shaders/Render.hlsl"));
	Skybox::shaders = gpu->newShaders(kl::file::readString("source/Shaders/Skybox.hlsl"));

	sphereMesh = gpu->newVertexBuffer("resource/meshes/sphere.obj");
	Skybox::box = gpu->newVertexBuffer("resource/meshes/cube.obj");

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

	skybox = kl::make<Render::Skybox>(kl::image(std::string("resource/textures/stars_milky.png")));

	camera.position -= camera.forward() * 2.0f;

	timer.reset();
}

struct VS_CB {
	kl::mat4 wMatrix;
	kl::mat4 vpMatrix;
};

struct PS_CB {
	kl::float4 sunDirection;
	kl::float4 cameraPosition;
	kl::float4 miscData;
};

void Render::Update() {
	Input::Scroll();

	deltaT = (float)timer.interval();
	elapsedT = (float)timer.elapsed();

	gpu->clear(kl::colors::gray);

	gpu->bind(skyboxDepth);
	skybox->render(camera.matrix());

	gpu->bind(defaultDepth);
	gpu->bind(renderShaders);

	VS_CB vsData = {};
	vsData.wMatrix = kl::mat4::rotation(Game::sphereRotation);
	vsData.vpMatrix = camera.matrix();
	gpu->autoVertexCBuffer(vsData);

	PS_CB psData = {};
	psData.sunDirection = { Game::sunDirection.normalize(), 1.0f };
	psData.cameraPosition = { camera.position, 1.0f };
	psData.miscData = {
		elapsedT / 1800.0f,
		0.0f,
		0.0f,
		0.0f
	};
	gpu->autoPixelCBuffer(psData);

	gpu->bindPixelShaderView(earthDayTexture, 0);
	gpu->bindPixelShaderView(earthNightTexture, 1);
	gpu->bindPixelShaderView(earthCloudsTexture, 2);
	gpu->bindPixelShaderView(earthNormalMap, 3);
	gpu->bindPixelShaderView(earthRoughnessMap, 4);

	gpu->draw(sphereMesh);

	gpu->swap(true);

	window.title(kl::format(int(1.0f / deltaT)));
}
