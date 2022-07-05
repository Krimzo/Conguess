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

	Game::Log("Compiling render shaders");
	shaders = Game::gpu->newShaders(kl::file::readString("source/Shaders/Render.hlsl"));

	Game::Log("Creating render targets");
	Render::Resize(Game::window.size());

	Game::Log("Loading box mesh");
	mesh = Game::gpu->newVertexBuffer("resource/meshes/sphere.obj");

	Game::Log("Loading earth day texture");
	earthDayTexture = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_day.png"))));

	Game::Log("Loading earth night texture");
	earthNightTexture = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_night.png"))));

	Game::Log("Loading earth clouds texture");
	earthCloudsTexture = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_clouds.png"))));

	Game::Log("Loading earth normal map");
	earthNormalMap = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_normal.png"))));

	Game::Log("Loading earth roughness map");
	earthRoughnessMap = Game::gpu->newShaderView(Game::gpu->newTexture(
		kl::image(std::string("resource/textures/earth_roughness.png"))));
}

void Render::Update() {
	Game::gpu->clear(Render::indexTargetView, {});

	Game::gpu->bindTargets({ Render::renderTargetView, Render::indexTargetView });
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

void Render::Resize(const kl::uint2& newSize) {
	Game::gpu->bindInternal({});
	Game::gpu->bindPixelShaderView(nullptr, 0);
	Game::gpu->bindPixelShaderView(nullptr, 1);

	kl::dx::desc::texture renderTextureDesc = {};
	renderTextureDesc.Width = newSize.x;
	renderTextureDesc.Height = newSize.y;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	Game::gpu->destroy(renderTargetView);
	Game::gpu->destroy(renderShaderView);
	kl::dx::texture renderTexture = Game::gpu->newTexture(&renderTextureDesc);
	renderTargetView = Game::gpu->newTargetView(renderTexture);
	renderShaderView = Game::gpu->newShaderView(renderTexture);
	Game::gpu->destroy(renderTexture);

	Game::gpu->destroy(indexTargetView);
	Game::gpu->destroy(indexShaderView);
	kl::dx::texture indexTexture = Game::gpu->newTexture(&renderTextureDesc);
	indexTargetView = Game::gpu->newTargetView(indexTexture);
	indexShaderView = Game::gpu->newShaderView(indexTexture);
	Game::gpu->destroy(indexTexture);
}
