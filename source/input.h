#pragma once

#include "helper.h"


struct ConguessInput
{
    Conguess& conguess;

    kl::Float2 mouse_geo_location{};
    int mouse_country_index = -1;

    kl::Float2 original_rotations;
    kl::Int2 original_mouse_pos;

    ConguessInput( Conguess& conguess );

    void update();

private:
    void new_random_country();
};
