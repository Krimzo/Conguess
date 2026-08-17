#pragma once

#include "logger.h"
#include "country_data.h"
#include "input.h"
#include "game.h"
#include "skybox.h"
#include "earth.h"
#include "postprocess.h"
#include "ui.h"


struct Conguess
{
    kl::Window window{ "Conguess" };
    kl::GPU gpu{ window.ptr() };
    kl::Timer timer{};
    kl::Camera camera{};

    kl::dx::TargetView render_target_view;
    kl::dx::ShaderView render_shader_view;
    kl::dx::TargetView info_target_view;
    kl::dx::ShaderView info_shader_view;

    ConguessCountryData country_data{ *this };
    ConguessInput input{ *this };
    ConguessGame game{ *this };
    ConguessSkybox skybox{ *this };
    ConguessEarth earth{ *this };
    ConguessPostprocess postprocess{ *this };
    ConguessUI ui{ *this };

    Conguess();

    bool update();

private:
    void resize( kl::Int2 size );
};
