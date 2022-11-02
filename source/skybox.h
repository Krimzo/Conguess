#pragma once

#include "klib.h"


namespace skybox
{
	inline kl::dx::depth_state depth_state = nullptr;
	inline kl::shaders shaders = {};
	inline kl::dx::buffer mesh = nullptr;
	inline kl::dx::shader_view texture = nullptr;

	void initialize();
	void update();
}
