#pragma once

#include "KrimzLib.h"


namespace Skybox {
	inline kl::dx::state::depth depthState = nullptr;
	inline kl::shaders shaders = {};
	inline kl::dx::mesh mesh = nullptr;
	inline kl::dx::view::shader texture = nullptr;

	void Initialize();
	void Update();
}
