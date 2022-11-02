#include "game.h"

#include "input.h"
#include "render.h"
#include "skybox.h"
#include "postprocess.h"
#include "data.h"


static constexpr bool debug_times = false;
static constexpr bool log_to_console = true;

void game::log(const std::string& message)
{
	if constexpr (log_to_console) {
		kl::print(message);
	}
	else {
		window->set_title(message);
	}
}

void game::log_play_stats()
{
	game::log(kl::format(
		"[", game::player_score, "] ",
		"(", data::countries[game::last_random_country].name, ")"
	));
}

void game::new_random_country()
{
	int new_random_country = last_random_country;
	while (new_random_country == last_random_country) {
		new_random_country = kl::random::get_int(static_cast<int>(data::countries.size()));
	}
	last_random_country = new_random_country;
}

int main()
{
	// Start
	game::window = kl::make<kl::window>(kl::uint2(1600, 900), "Country Guesser");
	game::window->on_resize = [&](const kl::uint2 new_size) {
		if (game::gpu && new_size.x > 0 && new_size.y > 0) {
			render::resize(new_size);
			game::gpu->resize_internal(new_size);
			game::gpu->set_viewport(new_size);
			game::camera.update_aspect_ratio(new_size);
		}
	};

	game::window->draw_image(kl::image(game::window->size(), kl::colors::gray));
	game::window->set_icon("resource/textures/icon.ico");

	game::gpu = kl::make<kl::gpu>(game::window->get_window());
	game::gpu->bind_sampler_state(game::gpu->new_sampler_state(true, false), 0);

	game::camera.position = game::camera.get_forward() * -2.0f;

	data::initialize();

	input::initialize();
	skybox::initialize();
	render::initialize();
	postprocess::initialize();

#ifndef _DEBUG
	kl::console::set_enabled(false);
#endif

	game::window->keyboard.r.on_press();

	// Update
	kl::timer timer = {};
	while (game::window->process(false)) {
		timer.update_interval();
		
		kl::time::get_interval();
		input::update();
		const float input_time = kl::time::get_interval();

		game::gpu->clear_internal();

		kl::time::get_interval();
		skybox::update();
		const float skybox_time = kl::time::get_interval();

		kl::time::get_interval();
		render::update();
		const float render_time = kl::time::get_interval();

		kl::time::get_interval();
		postprocess::update();
		const float postprocess_time = kl::time::get_interval();

		game::gpu->swap_buffers(true);

		if constexpr (debug_times) {
			game::log(kl::format(std::fixed, std::setprecision(2),
				"Input[", input_time * 1000.0f, "] ",
				"Skybox[", skybox_time * 1000.0f, "] ",
				"Render[", render_time * 1000.0f, "] ",
				"Postprocess[", postprocess_time * 1000.0f, "] ",
				"FPS(", static_cast<int>(1.0f / game::timer.get_interval()), ")"
			));
		}
	}
}
