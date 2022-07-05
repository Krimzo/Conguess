#pragma once

#include "KrimzLib.h"


namespace Render {
	class Skybox {
	private:
		kl::dx::view::shader m_ShaderView = nullptr;

	public:
		static kl::dx::mesh box;
		static kl::shaders shaders;

		Skybox(const kl::image& front, const kl::image& back, const kl::image& left, const kl::image& right, const kl::image& top, const kl::image& bottom);
		Skybox(const kl::image& fullbox);
		Skybox(const Render::Skybox&) = delete;
		void operator=(const Render::Skybox&) = delete;
		~Skybox();

		bool isValid() const;
		void render(const kl::mat4& vpMat) const;
	};
}
