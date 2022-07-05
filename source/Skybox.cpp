#include "Skybox.h"
#include "Game.h"


kl::dx::view::shader ProcessBoxImage(const kl::image& image) {
	if (image.width() % 4 == 0 && image.height() % 3 == 0) {
		const uint partWidth = image.width() / 4;
		const uint partHeight = image.height() / 3;
		if (partWidth == partHeight) {
			const kl::uint2 partSize(partWidth, partHeight);
			const kl::image front = image.rect(partSize * kl::uint2(1, 1), partSize * kl::uint2(2, 2));
			const kl::image back = image.rect(partSize * kl::uint2(3, 1), partSize * kl::uint2(4, 2));
			const kl::image left = image.rect(partSize * kl::uint2(0, 1), partSize * kl::uint2(1, 2));
			const kl::image right = image.rect(partSize * kl::uint2(2, 1), partSize * kl::uint2(3, 2));
			const kl::image top = image.rect(partSize * kl::uint2(1, 0), partSize * kl::uint2(2, 1));
			const kl::image bottom = image.rect(partSize * kl::uint2(1, 2), partSize * kl::uint2(2, 3));
			return Game::gpu->newShaderView(Game::gpu->newTexture(front, back, left, right, top, bottom));
		}
	}
	kl::console::error(true, "Texture has unsupported ratio!");
	return nullptr;
}

void Skybox::Initialize() {
	depthState = Game::gpu->newDepthState(false, false, false);

	Game::Log("Compiling skybox shaders");
	shaders = Game::gpu->newShaders(kl::file::readString("source/Shaders/Skybox.hlsl"));

	Game::Log("Loading box mesh");
	mesh = Game::gpu->newVertexBuffer("resource/meshes/cube.obj");

	Game::Log("Loading skybox texture");
	texture = ProcessBoxImage(kl::image(std::string("resource/textures/stars_milky.png")));
}

void Skybox::Update() {
	Game::gpu->bind(depthState);
	Game::gpu->bind(shaders);

	Game::gpu->autoVertexCBuffer(Game::camera.matrix());
	Game::gpu->bindPixelShaderView(texture, 0);
	Game::gpu->draw(mesh);
}
