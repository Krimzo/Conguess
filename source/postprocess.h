#pragma once

#include "klib.h"


namespace postprocess
{
	inline kl::dx::depth_state depth_state = nullptr;
	inline kl::shaders shaders = {};
	inline kl::dx::buffer mesh = nullptr;
	
	inline bool render_bounds = false;

	void initialize();
	void update();
}
