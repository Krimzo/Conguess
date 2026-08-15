#pragma once

#include "klibrary.h"


struct Conguess;

template<typename... Args>
constexpr void log( Args&&... args )
{
    kl::print( args... );
}

template<typename... Args>
constexpr void log_error( Args&&... args )
{
    kl::print( kl::colors::RED, args..., kl::colors::CONSOLE );
}

template<typename... Args>
constexpr void log_success( Args&&... args )
{
    kl::print( kl::colors::GREEN, args..., kl::colors::CONSOLE );
}

void load_shaders( kl::GPU& gpu, std::string_view const& name, kl::Shaders& out_shaders );
void load_mesh( kl::GPU& gpu, std::string_view const& name, kl::dx::Buffer& out_mesh );
void load_texture( kl::GPU& gpu, std::string_view const& name, kl::dx::ShaderView& out_texture_sv );
