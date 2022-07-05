#include "Game.h"
#include "Input.h"
#include "Render.h"
#include "Skybox.h"
#include "Postprocess.h"


static kl::timer timer = {};

void Game::Log(const std::string& message) {
	window.title(message);
}

void Start() {
	Game::window.draw(kl::image(Game::window.size(), kl::colors::gray));

	Game::window.icon("resource/textures/icon.ico");

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

	kl::time::interval();
	Skybox::Update();
	const double skyboxTime = kl::time::interval();

	kl::time::interval();
	Render::Update();
	const double renderTime = kl::time::interval();

	kl::time::interval();
	Postprocess::Update();
	const double postprocessTime = kl::time::interval();

	Game::gpu->swap(true);

	Game::Log(kl::format(std::fixed, std::setprecision(2),
		"Skybox[", skyboxTime * 1e3, "] ",
		"Render[", renderTime * 1e3, "] ",
		"Postprocess[", postprocessTime * 1e3, "] ",
		"Frame(", Game::deltaT * 1e3, ")"
	));
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
