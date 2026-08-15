#pragma once

#include "helper.h"


struct ConguessEarth
{
    Conguess& conguess;

    kl::dx::DepthState depth_state;
    kl::dx::Buffer mesh;
    kl::Shaders shaders;

    kl::dx::ShaderView earth_day_texture;
    kl::dx::ShaderView earth_night_texture;
    kl::dx::ShaderView earth_clouds_texture;
    kl::dx::ShaderView earth_normal_map;
    kl::dx::ShaderView earth_roughness_map;
    kl::dx::ShaderView earth_boundaries_map;
    kl::dx::ShaderView earth_indices_map;

    bool render_clouds = true;

    ConguessEarth( Conguess& conguess );

    void update();
};
