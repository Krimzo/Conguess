#pragma once

#include "logger.h"


void load_shaders( kl::GPU& gpu, std::string_view const& name, kl::Shaders& out_shaders );
void load_mesh( kl::GPU& gpu, std::string_view const& name, kl::dx::Buffer& out_mesh );
void load_texture( kl::GPU& gpu, std::string_view const& name, kl::dx::ShaderView& out_texture_sv );
