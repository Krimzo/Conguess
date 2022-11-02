#pragma once

#include "klib.h"


struct polygon
{
	std::vector<kl::float2> coords = {};

	[[nodiscard]] bool contains(const kl::float2& point) const
	{
		bool in = false;
		for (uint64_t i = 0, j = coords.size() - 1; i < coords.size(); j = i++) {
			if (((coords[i].y > point.y) != (coords[j].y > point.y)) && (point.x < (coords[j].x - coords[i].x) * (point.
				y - coords[i].y) / (coords[j].y - coords[i].y) + coords[i].x)) {
				in = !in;
			}
		}
		return in;
	}
};

struct country
{
	std::string name = {};
	std::vector<polygon> polygons = {};
};

namespace data
{
	inline std::vector<country> countries;
	
	void initialize();
}
