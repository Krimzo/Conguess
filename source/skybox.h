#pragma once

#include "helper.h"


struct ConguessSkybox
{
    Conguess& conguess;

    kl::dx::DepthState depth_state{};
    kl::dx::Buffer mesh{};
    kl::dx::ShaderView texture{};
    kl::Shaders shaders{};

    ConguessSkybox( Conguess& conguess );

    void update();
};
