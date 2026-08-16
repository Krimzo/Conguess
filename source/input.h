#pragma once

#include "helper.h"


struct ConguessInput
{
    Conguess& conguess;

    kl::Float2 rotations{ 45.0f, 180.0f }; // x is for camera, y is for sphere
    float camera_distance = 2.0f;

    kl::Float2 original_rotations;
    kl::Int2 original_mouse_pos;

    std::optional<kl::Float2> mouse_geo_location;
    std::optional<int> mouse_country_index;

    ConguessInput( Conguess& conguess );

    void update();
};
