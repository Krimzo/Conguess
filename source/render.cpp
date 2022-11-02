#include "render.h"

#include "game.h"
#include "input.h"


struct vs_cb
{
	kl::mat4 w_matrix = {};
	kl::mat4 vp_matrix = {};
};

struct ps_cb
{
	kl::float4 sun_direction = {};
	kl::float4 camera_position = {};
	kl::float4 misc_data = {};
};

void render::resize(const kl::uint2& size)
{
	game::gpu->bind_internal_targets();
	game::gpu->bind_pixel_shader_view(nullptr, 0);
	game::gpu->bind_pixel_shader_view(nullptr, 1);

	kl::dx::texture_descriptor render_texture_desc = {};
	render_texture_desc.Width = size.x;
	render_texture_desc.Height = size.y;
	render_texture_desc.MipLevels = 1;
	render_texture_desc.ArraySize = 1;
	render_texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	render_texture_desc.SampleDesc.Count = 1;
	render_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	render_texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	game::gpu->destroy(render_target_view);
	game::gpu->destroy(render_shader_view);
	const kl::dx::texture render_texture = game::gpu->new_texture(&render_texture_desc);
	render_target_view = game::gpu->new_target_view(render_texture);
	render_shader_view = game::gpu->new_shader_view(render_texture);
	game::gpu->destroy(render_texture);

	game::gpu->destroy(index_target_view);
	game::gpu->destroy(index_shader_view);
	const kl::dx::texture index_texture = game::gpu->new_texture(&render_texture_desc);
	index_target_view = game::gpu->new_target_view(index_texture);
	index_shader_view = game::gpu->new_shader_view(index_texture);
	game::gpu->destroy(index_texture);
}

void render::initialize()
{
	depth_state = game::gpu->new_depth_state(true, false, false);

	game::log("Compiling render shaders");
	shaders = game::gpu->new_shaders(kl::files::read_string("source/shaders/Render.hlsl"));

	game::log("Creating render targets");
	resize(game::window->size());

	game::log("Loading box mesh");
	mesh = game::gpu->new_vertex_buffer("resource/meshes/sphere.obj", true);

	game::log("Loading earth day texture");
	earth_day_texture = game::gpu->new_shader_view(game::gpu->new_texture(
		kl::image(std::string("resource/textures/earth_day.png"))));

	game::log("Loading earth night texture");
	earth_night_texture = game::gpu->new_shader_view(game::gpu->new_texture(
		kl::image(std::string("resource/textures/earth_night.png"))));

	game::log("Loading earth clouds texture");
	earth_clouds_texture = game::gpu->new_shader_view(game::gpu->new_texture(
		kl::image(std::string("resource/textures/earth_clouds.png"))));

	game::log("Loading earth normal map");
	earth_normal_map = game::gpu->new_shader_view(game::gpu->new_texture(
		kl::image(std::string("resource/textures/earth_normal.png"))));

	game::log("Loading earth roughness map");
	earth_roughness_map = game::gpu->new_shader_view(game::gpu->new_texture(
		kl::image(std::string("resource/textures/earth_roughness.png"))));

	game::log("Loading earth boundaries map");
	earth_boundaries_map = game::gpu->new_shader_view(game::gpu->new_texture(
		kl::image(std::string("resource/textures/earth_boundaries.png"))));

	game::log("Loading earth indices map");
	earth_indices_map = game::gpu->new_shader_view(game::gpu->new_texture(
		kl::image(std::string("resource/textures/earth_indices.png"))));
}

void render::update()
{
	game::gpu->clear_target_view(index_target_view, kl::float4::splash(0.0f));

	game::gpu->bind_targets({ render_target_view, index_target_view });
	game::gpu->bind_depth_state(depth_state);

	game::gpu->bind_shaders(shaders);
	game::gpu->bind_pixel_shader_view(earth_day_texture, 0);
	game::gpu->bind_pixel_shader_view(earth_night_texture, 1);
	game::gpu->bind_pixel_shader_view(earth_clouds_texture, 2);
	game::gpu->bind_pixel_shader_view(earth_normal_map, 3);
	game::gpu->bind_pixel_shader_view(earth_roughness_map, 4);
	game::gpu->bind_pixel_shader_view(earth_boundaries_map, 5);
	game::gpu->bind_pixel_shader_view(earth_indices_map, 6);

	vs_cb vs_data = {};
	vs_data.w_matrix = kl::mat4::rotation(game::sphere_rotation);
	vs_data.vp_matrix = game::camera.matrix();
	game::gpu->set_vertex_const_buffer(vs_data);

	ps_cb ps_data = {};
	ps_data.sun_direction = { game::sun_direction.normalize(), 0.0f };
	ps_data.camera_position = { game::camera.position, 0.0f };
	ps_data.misc_data = {
		game::timer.get_elapsed() / 1800.0f,
		static_cast<float>(input::mouse_country_index + 1),
		static_cast<float>(render_clouds),
		0.0f
	};
	game::gpu->set_pixel_const_buffer(ps_data);

	game::gpu->draw_vertex_buffer(mesh);
}
