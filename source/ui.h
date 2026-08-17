#pragma once

#include "helper.h"


struct ConguessUI
{
    Conguess& conguess;

    kl::TextFormat text_format;
    float font_size = 25.0f;

    ConguessUI( Conguess& conguess );

    void update();
};
