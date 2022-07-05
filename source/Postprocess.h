#pragma once

#include "KrimzLib.h"


namespace Postprocess {
	inline kl::dx::state::depth depthState = nullptr;
	inline kl::shaders shaders;

	inline kl::dx::mesh mesh = nullptr;

	void Initialize();
	void Update();
}
