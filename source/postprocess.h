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

    kl::Float4 hover_country_color_multi = { 1.75f, 1.75f, 1.75f, 1.0f };
    kl::Float4 hold_country_color_multi = { 2.0f, 2.0f, 2.0f, 1.0f };
    kl::Float4 border_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool render_borders = false;
    bool render_atmosphere = true;

    ConguessPostprocess( Conguess& conguess );

    void update();
};
