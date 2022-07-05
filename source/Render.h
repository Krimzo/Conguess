#pragma once

#include "KrimzLib.h"


namespace Render {
	inline kl::dx::state::depth depthState = nullptr;
	inline kl::shaders shaders;

	inline kl::dx::view::target renderTargetView = nullptr;
	inline kl::dx::view::shader renderShaderView = nullptr;
	inline kl::dx::view::target indexTargetView = nullptr;
	inline kl::dx::view::shader indexShaderView = nullptr;

	inline kl::dx::mesh mesh = nullptr;
	inline kl::dx::view::shader earthDayTexture = nullptr;
	inline kl::dx::view::shader earthNightTexture = nullptr;
	inline kl::dx::view::shader earthCloudsTexture = nullptr;
	inline kl::dx::view::shader earthNormalMap = nullptr;
	inline kl::dx::view::shader earthRoughnessMap = nullptr;
	inline kl::dx::view::shader starsMilkyTexture = nullptr;


	void Initialize();
	void Update();
	void Resize(const kl::uint2& newSize);
}
