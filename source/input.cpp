#include "input.h"

#include "game.h"
#include "data.h"
#include "render.h"
#include "postprocess.h"


static bool last_intersect = false;
static kl::float3 last_direction = {};
static const kl::sphere sphere = { {}, 1.0f };

static kl::ray get_mouse_ray()
{
	const auto frame_size = kl::float2(game::window->size());
	const kl::float2 ndc = {
		static_cast<float>(game::window->mouse.position().x),
		static_cast<float>(frame_size.y - game::window->mouse.position().y)
	};
	return { game::camera, ndc / frame_size * 2.0f - kl::float2(1.0f, 1.0f) };
}

static void save_last_values()
{
	last_intersect = get_mouse_ray().intersect_sphere(sphere, last_direction);
	last_direction = last_direction.normalize();
}

void input::initialize()
{
	// Mouse
	game::window->mouse.left.on_press = [&]
	{
		save_last_values();
	};
	
	game::window->mouse.left.on_down = [&]
	{
		if (kl::float3 current_direction; last_intersect && get_mouse_ray().intersect_sphere(sphere, current_direction)) {
			current_direction = current_direction.normalize();

			const kl::float2 delta_angles = {
				kl::float2(last_direction.z, last_direction.y).angle({ current_direction.z, current_direction.y }, true),
				kl::float2(last_direction.x, last_direction.z).angle({ current_direction.x, current_direction.z }, true)
			};

			game::sphere_rotation.y -= delta_angles.y;

			const float camera_angle_x = kl::math::to_degrees(std::asin(game::camera.get_forward().y));
			const float new_camera_angle_x = kl::math::minmax(camera_angle_x - delta_angles.x, -85.0f, 85.0f);
			const float new_camera_angle_x_rads = kl::math::to_radians(new_camera_angle_x);
			game::camera.set_forward({ 0.0f, std::sin(new_camera_angle_x_rads), std::cos(new_camera_angle_x_rads) });
			game::camera.position = game::camera.get_forward() * -2.0f;

			save_last_values();
		}
	};
	
	game::window->mouse.left.on_release = [&]
	{
		if (mouse_country_index == game::last_random_country) {
			game::new_random_country();
			game::player_score++;
		}
		else if (mouse_country_index >= 0) {
			game::player_score--;
		}
		game::log_play_stats();
	};
	
	game::window->mouse.middle.on_press = game::window->mouse.left.on_press;
	game::window->mouse.right.on_press = game::window->mouse.left.on_press;
	game::window->mouse.middle.on_down = game::window->mouse.left.on_down;
	game::window->mouse.right.on_down = game::window->mouse.left.on_down;

	// Keyboard
	game::window->keyboard.r.on_press = [&]() {
		game::new_random_country();
		game::player_score = 0;
		game::log_play_stats();
	};
	game::window->keyboard.c.on_press = [&]() {
		render::render_clouds = !render::render_clouds;
	};
	game::window->keyboard.b.on_press = [&]() {
		postprocess::render_bounds = !postprocess::render_bounds;
	};
}

void input::update()
{
	static int last_scroll = 0;
	const int scroll_delta = last_scroll - game::window->mouse.scroll();
	game::camera.field_of_view = kl::math::minmax(game::camera.field_of_view + scroll_delta * 5.0f, 5.0f, 90.0f);
	last_scroll = game::window->mouse.scroll();

	kl::float3 mouse_sphere_intersect = {};
	get_mouse_ray().intersect_sphere(sphere, mouse_sphere_intersect);

	const auto mouse_sphere_intersect_no_y = kl::float3(mouse_sphere_intersect.x, 0.0f, mouse_sphere_intersect.z);
	mouse_geo_location.x = mouse_sphere_intersect.angle(mouse_sphere_intersect_no_y);
	mouse_geo_location.x *= (mouse_sphere_intersect.y < 0.0f) ? -1.0f : 1.0f;

	const kl::float3 greenwich = (kl::mat4::rotation(game::sphere_rotation) * kl::float4(1.0f, 0.0f, 0.0f, 1.0f)).xyz;
	mouse_geo_location.y = kl::float2(greenwich.x, greenwich.z).angle(kl::float2(mouse_sphere_intersect_no_y.x, mouse_sphere_intersect_no_y.z), true);

	for (int i = 0; i < data::countries.size(); i++) {
		for (auto& polygon : data::countries[i].polygons) {
			if (polygon.contains(mouse_geo_location)) {
				mouse_country_index = i;
				return;
			}
		}
	}
	mouse_country_index = -10;
}
