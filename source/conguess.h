#pragma once

#include "country_data.h"
#include "input.h"
#include "skybox.h"
#include "earth.h"
#include "postprocess.h"


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

    kl::Float3 sun_direction{ 1.0f, -1.0f, 1.0f };
    kl::Float2 rotations{ 45.0f, 180.0f }; // x is for camera, y is for sphere
    float camera_distance = 2.0f;
    int game_random_country = -1;
    int player_score = 0;

    ConguessCountryData country_data{ *this };
    ConguessInput input{ *this };
    ConguessSkybox skybox{ *this };
    ConguessEarth earth{ *this };
    ConguessPostprocess postprocess{ *this };

    Conguess();

    bool update();

private:
    void resize( kl::Int2 size );
    void update_camera();
};
