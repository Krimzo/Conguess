#pragma once

#include "helper.h"


struct ConguessUI
{
    Conguess& conguess;

    kl::TextFormat log_text_format;
    kl::TextFormat keybind_text_format;
    float log_font_size = 25.0f;
    float keybind_font_size = 10.0f;

    ConguessUI( Conguess& conguess );

    void update();
};
