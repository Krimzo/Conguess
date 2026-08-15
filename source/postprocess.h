#pragma once

#include "helper.h"


struct ConguessPostprocess
{
    Conguess& conguess;

    kl::dx::DepthState depth_state;
    kl::Shaders shaders;
    kl::dx::Buffer mesh;

    bool render_bounds = false;

    ConguessPostprocess( Conguess& conguess );

    void update();
};
