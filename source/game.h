#pragma once

#include "klib.h"


namespace game
{
	inline kl::ref<kl::window> window = {};
	inline kl::ref<kl::gpu> gpu = {};
	inline kl::camera camera = {};
	inline kl::timer timer = {};

	inline kl::float3 sphere_rotation = {};
	inline kl::float3 sun_direction = { 1.0f, -1.0f, 1.0f };

	inline int last_random_country = -1;
	inline int player_score = 0;

	void log(const std::string& message);
	void log_play_stats();
	void new_random_country();
}
