#pragma once

#include "KrimzLib.h"


namespace Input {
	inline kl::float2 mouseGeoLocation;
	inline int mouseCountryIndex = -1;

	void Initialize();
	void Update();
}
