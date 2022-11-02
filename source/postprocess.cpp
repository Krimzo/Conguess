#include "postprocess.h"

#include "render.h"
#include "game.h"


void postprocess::initialize()
{
	depth_state = game::gpu->new_depth_state(false, false, false);

	game::log("Compiling postprocess shaders");
	shaders = game::gpu->new_shaders(kl::files::read_string("source/shaders/postprocess.hlsl"));

	game::log("Loading screen mesh");
	mesh = game::gpu->generate_screen_mesh();
}

void postprocess::update()
{
	game::gpu->bind_internal_targets();
	game::gpu->bind_depth_state(depth_state);
	game::gpu->bind_shaders(shaders);

	game::gpu->bind_pixel_shader_view(render::render_shader_view, 0);
	game::gpu->bind_pixel_shader_view(render::index_shader_view, 1);

	const kl::float4 misc_data = {
		static_cast<float>(render_bounds),
		static_cast<float>(game::window->mouse.left.state()),
		0.0f,
		0.0f
	};
	game::gpu->set_pixel_const_buffer(misc_data);

	game::gpu->draw_vertex_buffer(mesh);
}
