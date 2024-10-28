#include "skybox.h"

#include "render.h"
#include "game.h"


static kl::dx::shader_view process_box_image(const kl::image& image)
{
	if (image.width() % 4 == 0 && image.height() % 3 == 0) {
		const uint32_t part_width = image.width() / 4;
		if (const uint32_t part_height = image.height() / 3; part_width == part_height) {
			const kl::uint2 part_size(part_width, part_height);
			const kl::image front = image.get_square(part_size * kl::uint2(1, 1), part_size * kl::uint2(2, 2));
			const kl::image back = image.get_square(part_size * kl::uint2(3, 1), part_size * kl::uint2(4, 2));
			const kl::image left = image.get_square(part_size * kl::uint2(0, 1), part_size * kl::uint2(1, 2));
			const kl::image right = image.get_square(part_size * kl::uint2(2, 1), part_size * kl::uint2(3, 2));
			const kl::image top = image.get_square(part_size * kl::uint2(1, 0), part_size * kl::uint2(2, 1));
			const kl::image bottom = image.get_square(part_size * kl::uint2(1, 2), part_size * kl::uint2(2, 3));
			return game::gpu->new_shader_view(game::gpu->new_texture(front, back, left, right, top, bottom));
		}
	}
	kl::assert(false, "Texture has unsupported ratio!");
	return nullptr;
}

void skybox::initialize()
{
	depth_state = game::gpu->new_depth_state(false, false, false);

	game::log("Compiling skybox shaders");
	shaders = game::gpu->new_shaders(kl::files::read_string("source/shaders/skybox.hlsl"));

	game::log("Loading box mesh");
	mesh = game::gpu->new_vertex_buffer("resource/meshes/cube.obj", true);

	game::log("Loading skybox texture");
	texture = process_box_image(kl::image(std::string("resource/textures/stars_milky.png")));
}

void skybox::update()
{
	game::gpu->clear_target_view(render::render_target_view, kl::float4(kl::colors::gray));

	game::gpu->bind_targets({ render::render_target_view });
	game::gpu->bind_depth_state(depth_state);
	
	game::gpu->bind_shaders(shaders);
	game::gpu->bind_pixel_shader_view(texture, 0);
	game::gpu->set_vertex_const_buffer(game::camera.matrix());
	
	game::gpu->draw_vertex_buffer(mesh);
}
