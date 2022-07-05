#include "Skybox.h"
#include "Render.h"


kl::dx::mesh Render::Skybox::box = nullptr;
kl::shaders Render::Skybox::shaders = {};

Render::Skybox::Skybox(const kl::image& front, const kl::image& back, const kl::image& left, const kl::image& right, const kl::image& top, const kl::image& bottom) {
	kl::dx::texture boxTex = Render::gpu->newTexture(front, back, left, right, top, bottom);
	m_ShaderView = Render::gpu->newShaderView(boxTex);
	Render::gpu->destroy(boxTex);
}
Render::Skybox::Skybox(const kl::image& fullbox) {
	if (fullbox.width() % 4 == 0 && fullbox.height() % 3 == 0) {
		const uint partWidth = fullbox.width() / 4;
		const uint partHeight = fullbox.height() / 3;
		if (partWidth == partHeight) {
			const kl::uint2 partSize(partWidth, partHeight);
			const kl::image front = fullbox.rect(partSize * kl::uint2(1, 1), partSize * kl::uint2(2, 2));
			const kl::image back = fullbox.rect(partSize * kl::uint2(3, 1), partSize * kl::uint2(4, 2));
			const kl::image left = fullbox.rect(partSize * kl::uint2(0, 1), partSize * kl::uint2(1, 2));
			const kl::image right = fullbox.rect(partSize * kl::uint2(2, 1), partSize * kl::uint2(3, 2));
			const kl::image top = fullbox.rect(partSize * kl::uint2(1, 0), partSize * kl::uint2(2, 1));
			const kl::image bottom = fullbox.rect(partSize * kl::uint2(1, 2), partSize * kl::uint2(2, 3));
			this->Skybox::Skybox(front, back, left, right, top, bottom);
		}
	}
	kl::console::error(!m_ShaderView, "Texture has unsupported ratio!");
}
Render::Skybox::~Skybox() {
	if (Render::gpu && isValid()) {
		Render::gpu->destroy(m_ShaderView);
	}
}

bool Render::Skybox::isValid() const {
	return bool(m_ShaderView);
}

void Render::Skybox::render(const kl::mat4& vpMat) const {
	if (m_ShaderView) {
		Render::gpu->bind(shaders);
		Render::gpu->autoVertexCBuffer(vpMat);
		Render::gpu->bindPixelShaderView(m_ShaderView, 0);
		Render::gpu->draw(box);
	}
}
