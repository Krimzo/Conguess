#pragma once

#include "klib.h"


namespace render
{
	inline kl::dx::depth_state depth_state = nullptr;
	inline kl::dx::buffer mesh = nullptr;
	inline kl::shaders shaders;

	inline kl::dx::target_view render_target_view = nullptr;
	inline kl::dx::shader_view render_shader_view = nullptr;
	inline kl::dx::target_view index_target_view = nullptr;
	inline kl::dx::shader_view index_shader_view = nullptr;
	
	inline kl::dx::shader_view earth_day_texture = nullptr;
	inline kl::dx::shader_view earth_night_texture = nullptr;
	inline kl::dx::shader_view earth_clouds_texture = nullptr;
	inline kl::dx::shader_view earth_normal_map = nullptr;
	inline kl::dx::shader_view earth_roughness_map = nullptr;
	inline kl::dx::shader_view earth_boundaries_map = nullptr;
	inline kl::dx::shader_view earth_indices_map = nullptr;

	inline bool render_clouds = true;

	void resize(const kl::uint2& size);
	void initialize();
	void update();
}
