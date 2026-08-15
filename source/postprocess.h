#pragma once

#include "helper.h"


struct ConguessPostprocess
{
    Conguess& conguess;

    kl::dx::Buffer mesh;
    kl::dx::RasterState raster_state;
    kl::dx::DepthState depth_state;
    kl::Shaders shaders;
    kl::dx::SamplerState sampler;

    bool render_bounds = false;

    ConguessPostprocess( Conguess& conguess );

    void update();
};
