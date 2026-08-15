#pragma once

#include "helper.h"


struct ConguessEarth
{
    Conguess& conguess;

    kl::dx::DepthState depth_state;
    kl::dx::Buffer mesh;
    kl::Shaders shaders;

    kl::dx::ShaderView earth_day_sv;
    kl::dx::ShaderView earth_night_sv;
    kl::dx::ShaderView earth_clouds_sv;
    kl::dx::ShaderView earth_normal_sv;
    kl::dx::ShaderView earth_roughness_sv;
    kl::dx::ShaderView earth_boundaries_sv;
    kl::dx::ShaderView earth_indices_sv;

    bool render_clouds = true;

    ConguessEarth( Conguess& conguess );

    void update();
};
