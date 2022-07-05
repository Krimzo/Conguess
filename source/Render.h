#pragma once

#include "KrimzLib.h"


namespace Render {
	inline kl::window window;

	inline kl::reference<kl::gpu> gpu;

	inline kl::dx::state::depth defaultDepth = nullptr;
	inline kl::dx::state::depth skyboxDepth = nullptr;

	inline kl::shaders renderShaders;

	inline kl::dx::mesh sphereMesh = nullptr;

	inline kl::dx::view::shader earthDayTexture = nullptr;
	inline kl::dx::view::shader earthNightTexture = nullptr;
	inline kl::dx::view::shader earthCloudsTexture = nullptr;
	inline kl::dx::view::shader earthNormalMap = nullptr;
	inline kl::dx::view::shader earthRoughnessMap = nullptr;
	inline kl::dx::view::shader starsMilkyTexture = nullptr;

	inline kl::camera camera;

	inline kl::timer timer;
	inline float deltaT = 0.0f;
	inline float elapsedT = 0.0f;

	void Start();
	void Update();
}
