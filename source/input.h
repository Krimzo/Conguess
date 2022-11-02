#pragma once

#include "klib.h"


namespace input
{
	inline kl::float2 mouse_geo_location = {};
	inline int mouse_country_index = -1;

	void initialize();
	void update();
}
