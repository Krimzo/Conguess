#pragma once

#include "helper.h"


struct ConguessInput
{
    Conguess& conguess;

    kl::Float3 earth_rotation{ 0.0f, 180.0f, 0.0f };
    kl::Float3 camera_rotation{ 45.0f, 0.0f, 0.0f };
    float camera_distance = 1.5f;
    bool rotate_camera_y = false;

    kl::Float2 original_rotations;
    kl::Int2 original_mouse_pos;
    int original_mouse_country = -1;
    bool original_is_correct = false;

    std::optional<kl::Float2> mouse_geo_location;
    std::optional<int> mouse_country_index;

    ConguessInput( Conguess& conguess );

    void update();
};
