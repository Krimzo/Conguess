#pragma once

#include "KrimzLib.h"


namespace Game {
	inline kl::window window;
	inline kl::reference<kl::gpu> gpu;

	inline kl::camera camera;

	inline float deltaT = 0.0f;
	inline float elapsedT = 0.0f;

	inline kl::float3 sphereRotation = {};
	inline kl::float3 sunDirection = { 1.0f, -1.0f, 1.0f };

	inline int lastRandomCountry = -1;
	inline int playerScore = 0;

	void Log(const std::string& message);
	void LogPlayStats();
	void NewRandomCountry();
}
