#include "Game.h"
#include "Input.h"
#include "Render.h"
#include "Skybox.h"
#include "Postprocess.h"
#include "Data.h"


static constexpr bool DEBUG_TIMES = true;
static kl::timer timer = {};

void Game::Log(const std::string& message) {
	window.title(message);
}

void Resize(const kl::uint2& newSize) {
	if (newSize.x > 0 && newSize.y > 0) {
		Render::Resize(newSize);
		Game::gpu->regenInternal(newSize);
		Game::gpu->viewport(newSize);
		Game::camera.aspect = float(newSize.x) / newSize.y;
	}
}

void Start() {
	Game::window.draw(kl::image(Game::window.size(), kl::colors::gray));
	Game::window.icon("resource/textures/icon.ico");

	Game::gpu = kl::make<kl::gpu>(Game::window);
	Game::gpu->bind(Game::gpu->newSamplerState(true, false), 0);

	Game::window.resize = Resize;
	Game::camera.position = Game::camera.forward() * -2.0f;

	Data::Initialize();

	Input::Initialize();
	Skybox::Initialize();
	Render::Initialize();
	Postprocess::Initialize();

	Game::window.title("World Guesser");

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

	if constexpr (DEBUG_TIMES) {
		Game::Log(kl::format(std::fixed, std::setprecision(2),
			"Skybox[", skyboxTime * 1e3, "] ",
			"Render[", renderTime * 1e3, "] ",
			"Postprocess[", postprocessTime * 1e3, "] ",
			"Frame(", Game::deltaT * 1e3, ")"
		));
	}
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
