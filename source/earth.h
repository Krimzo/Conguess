#pragma once

#include "helper.h"


inline constexpr float4 NEUTRAL_HOLD_COLOR = { 2.0f, 2.0f, 2.0f, 1.0f };
inline constexpr float4 CORRECT_HOLD_COLOR = { 1.0f, 2.0f, 1.0f, 1.0f };
inline constexpr float4 WRONG_HOLD_COLOR = { 2.0f, 1.0f, 1.0f, 1.0f };

struct ConguessEarth
{
    Conguess& conguess;

    kl::dx::Buffer mesh;
    kl::dx::RasterState raster_state;
    kl::dx::DepthState depth_state;
    kl::Shaders shaders;
    kl::dx::ShaderView earth_day_sv;
    kl::dx::ShaderView earth_night_sv;
    kl::dx::ShaderView earth_clouds_sv;
    kl::dx::ShaderView earth_normal_sv;
    kl::dx::ShaderView earth_roughness_sv;
    kl::dx::ShaderView earth_borders_sv;
    kl::dx::ShaderView earth_indices_sv;
    kl::dx::SamplerState linear_sampler;
    kl::dx::SamplerState direct_sampler;

    float3 sun_direction{ 1.0f, -1.0f, 1.0f };
    bool render_clouds = true;

    ConguessEarth( Conguess& conguess );

    void update();
};
