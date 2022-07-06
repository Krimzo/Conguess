#pragma once

#include "KrimzLib.h"


namespace Postprocess {
	inline kl::dx::state::depth depthState = nullptr;
	inline kl::shaders shaders;

	inline kl::dx::mesh mesh = nullptr;

	inline bool renderBounds = false;

	void Initialize();
	void Update();
}
