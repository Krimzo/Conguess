#pragma once

#include "helper.h"


struct ConguessInput
{
    Conguess& conguess;

    std::optional<kl::Float2> mouse_geo_location;
    std::optional<int> mouse_country_index;

    kl::Float2 original_rotations;
    kl::Int2 original_mouse_pos;

    ConguessInput( Conguess& conguess );

    void update();

private:
    void new_random_country();
};
