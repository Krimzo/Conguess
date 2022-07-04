#include "Render.h"
#include "Input.h"


int main() {
	Render::window.start = Render::Start;
	Render::window.update = Render::Update;
	Render::window.end = [&]() {
		Render::gpu = nullptr;
	};

	Render::window.mouse.lmb.press = Input::LMBPress;
	Render::window.mouse.lmb.down = Input::LMBDown;

	Render::window.run({ 1600, 900 }, "World Guesser", true, true);
}
