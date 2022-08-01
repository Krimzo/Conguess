#include "Game.h"
#include "Input.h"
#include "Render.h"
#include "Skybox.h"
#include "Postprocess.h"
#include "Data.h"


static constexpr bool DEBUG_TIMES = false;

void Game::Log(const std::string& message) {
	window->title(message);
}

void Game::LogPlayStats() {
	Game::Log(kl::format(
		"[", Game::playerScore, "] ",
		"(", Data::countries[Game::lastRandomCountry].name, ")"
	));
}

void Game::NewRandomCountry() {
	int newRandomCountry = lastRandomCountry;
	while (newRandomCountry == lastRandomCountry) {
		newRandomCountry = kl::random::INT(int(Data::countries.size()));
	}
	lastRandomCountry = newRandomCountry;
}

int main() {
	Game::window = kl::make<kl::window>(kl::uint2(1600, 900), "Country Guesser");
	Game::window->resize = [&](kl::uint2 newSize) {
		if (Game::gpu && newSize.x > 0 && newSize.y > 0) {
			Render::Resize(newSize);
			Game::gpu->regenInternal(newSize);
			Game::gpu->viewport(newSize);
			Game::camera.resize(newSize);
		}
	};

	// Start
	Game::window->draw(kl::image(Game::window->size(), kl::colors::gray));
	Game::window->icon("resource/textures/icon.ico");

	Game::gpu = kl::make<kl::gpu>(*Game::window);
	Game::gpu->bind(Game::gpu->newSamplerState(true, false), 0);

	Game::camera.position = Game::camera.forward() * -2.0f;

	Data::Initialize();

	Input::Initialize();
	Skybox::Initialize();
	Render::Initialize();
	Postprocess::Initialize();

#ifndef _DEBUG
	kl::console::hide();
#endif

	Game::window->keys.r.press();

	// Update
	kl::timer timer;
	while (Game::window->process(false)) {
		kl::time::interval();
		Input::Update();
		const double inputTime = kl::time::interval();

		Game::deltaT = (float) timer.interval();
		Game::elapsedT = (float) timer.elapsed();

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
				"Input[", inputTime * 1000.0, "] ",
				"Skybox[", skyboxTime * 1000.0, "] ",
				"Render[", renderTime * 1000.0, "] ",
				"Postprocess[", postprocessTime * 1000.0, "] ",
				"FPS(", int(1.0 / Game::deltaT), ")"
			));
		}
	}
}
