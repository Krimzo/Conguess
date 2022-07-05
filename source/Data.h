#pragma once

#include "KrimzLib.h"


namespace Data {
	struct Polygon {
		std::vector<kl::float2> coords;

		bool contains(const kl::float2& point) const {
			bool in = false;
			for (uint64 i = 0, j = coords.size() - 1; i < coords.size(); j = i++) {
				if (((coords[i].y > point.y) != (coords[j].y > point.y)) && (point.x < (coords[j].x - coords[i].x) * (point.y - coords[i].y) / (coords[j].y - coords[i].y) + coords[i].x)) {
					in = !in;
				}
			}
			return in;
		}
	};
	struct Country {
		std::string name;
		std::vector<Polygon> polygons;
	};

	inline std::vector<Country> countries;

	void Initialize();
}
