#pragma once

#include "KrimzLib.h"


namespace Data {
	struct Polygon {
		std::vector<kl::float2> coords;
	};
	struct Country {
		std::string name;
		std::vector<Polygon> polygons;
	};

	inline std::vector<Country> countries;

	void Initialize();
}
