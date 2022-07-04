#pragma once

#include "KrimzLib.h"


namespace Render {
	inline kl::window window;

	inline kl::reference<kl::gpu> gpu;
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

	struct VS_CB {
		kl::mat4 wMatrix;
		kl::mat4 vpMatrix;
	};

	struct PS_CB {
		kl::float4 sunDirection;
		kl::float4 cameraPosition;
	};

	void Start();
	void Update();
}
