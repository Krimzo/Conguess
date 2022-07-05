#include "Postprocess.h"
#include "Render.h"
#include "Game.h"


void Postprocess::Initialize() {
	depthState = Game::gpu->newDepthState(false, false, false);

	Game::Log("Compiling postprocess shaders");
	shaders = Game::gpu->newShaders(kl::file::readString("source/shaders/Postprocess.hlsl"));

	Game::Log("Loading screen mesh");
	mesh = Game::gpu->newVertexBuffer("resource/meshes/screen.obj");
}

void Postprocess::Update() {
	Game::gpu->bindInternal();
	Game::gpu->bind(depthState);
	Game::gpu->bind(shaders);

	Game::gpu->bindPixelShaderView(Render::renderShaderView, 0);
	Game::gpu->bindPixelShaderView(Render::indexShaderView, 1);

	Game::gpu->draw(mesh);
}
