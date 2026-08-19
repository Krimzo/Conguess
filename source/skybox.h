#pragma once

#include "helper.h"


struct ConguessSkybox
{
    Conguess& conguess;

    kl::dx::Buffer mesh;
    kl::dx::RasterState raster_state;
    kl::Shaders shaders;
    kl::dx::ShaderView texture;
    kl::dx::SamplerState sampler;

    ConguessSkybox( Conguess& conguess );

    void update();
};
