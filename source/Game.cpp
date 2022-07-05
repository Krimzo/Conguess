#include "Game.h"
#include "Input.h"
#include "Render.h"
#include "Skybox.h"
#include "Postprocess.h"


static kl::timer timer = {};

void Start() {
	Game::gpu = kl::make<kl::gpu>(Game::window);
	Game::gpu->bind(Game::gpu->newSamplerState(true, false), 0);

	Game::window.resize = [&](const kl::uint2& newSize) {
		if (newSize.x > 0 && newSize.y > 0) {
			Game::gpu->regenInternal(newSize);
			Game::gpu->viewport({}, newSize);
			Game::camera.aspect = float(newSize.x) / newSize.y;
		}
	};

	Game::camera.position = Game::camera.forward() * -2.0f;

	Input::Initialize();
	Render::Initialize();
	Skybox::Initialize();
	Postprocess::Initialize();

	timer.reset();
}

void Update() {
	Input::Update();

	Game::deltaT = (float)timer.interval();
	Game::elapsedT = (float)timer.elapsed();

	Game::gpu->clear(kl::colors::gray);

	Skybox::Update();
	Render::Update();
	Postprocess::Update();

	Game::gpu->swap(true);

	Game::window.title(kl::format(int(1.0f / Game::deltaT)));
}

void End() {
	Game::gpu = nullptr;
}

int main() {
	Game::window.start = Start;
	Game::window.update = Update;
	Game::window.end = End;
	Game::window.run({ 1600, 900 }, "World Guesser", true, true);
}
