#include "conguess.h"


ConguessEarth::ConguessEarth( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    mesh = gpu.create_sphere_mesh( 1.0f, 25, 50, true );
    raster_state = gpu.create_raster_state( false, true );
    depth_state = gpu.create_depth_state( true, false, false );
    load_shaders( gpu, "earth", shaders );
    load_texture( gpu, "earth_day", earth_day_sv );
    load_texture( gpu, "earth_night", earth_night_sv );
    load_texture( gpu, "earth_clouds", earth_clouds_sv );
    load_texture( gpu, "earth_normal", earth_normal_sv );
    load_texture( gpu, "earth_roughness", earth_roughness_sv );
    load_texture( gpu, "earth_borders", earth_borders_sv );
    load_texture( gpu, "earth_indices", earth_indices_sv );
    linear_sampler = gpu.create_sampler_state( true, false );
    direct_sampler = gpu.create_sampler_state( false, false );
}

void ConguessEarth::update()
{
    auto& gpu = conguess.gpu;

    gpu.bind_target_depth_views( { conguess.render_target_view.get(), conguess.info_target_view.get() },
        gpu.back_depth_view() );
    gpu.bind_raster_state( raster_state );
    gpu.bind_depth_state( depth_state );

    gpu.bind_shader_view_for_pixel_shader( earth_day_sv, 0 );
    gpu.bind_shader_view_for_pixel_shader( earth_night_sv, 1 );
    gpu.bind_shader_view_for_pixel_shader( earth_clouds_sv, 2 );
    gpu.bind_shader_view_for_pixel_shader( earth_normal_sv, 3 );
    gpu.bind_shader_view_for_pixel_shader( earth_roughness_sv, 4 );
    gpu.bind_shader_view_for_pixel_shader( earth_borders_sv, 5 );
    gpu.bind_shader_view_for_pixel_shader( earth_indices_sv, 6 );
    gpu.bind_sampler_state_for_pixel_shader( linear_sampler, 0 );
    gpu.bind_sampler_state_for_pixel_shader( direct_sampler, 1 );

    struct alignas( 16 ) CB
    {
        kl::Float4x4 W;
        kl::Float4x4 VP;
        kl::Float3 SUN_DIRECTION;
        float ELAPSED_TIME;
        kl::Float3 CAMERA_POSITION;
        float RENDER_CLOUDS;
        int MOUSE_COUNTRY;
    } cb = {};

    cb.W = kl::Float4x4::rotation( { 0.0f, -conguess.input.rotations.y, 0.0f } );
    cb.VP = conguess.camera.matrix();
    cb.SUN_DIRECTION = kl::normalize( sun_direction );
    cb.ELAPSED_TIME = conguess.timer.elapsed() / 1800.0f;
    cb.CAMERA_POSITION = conguess.camera.position;
    cb.RENDER_CLOUDS = render_clouds;
    cb.MOUSE_COUNTRY = conguess.input.mouse_country_index.value_or( -1 );

    shaders.upload( cb );
    gpu.bind_shaders( shaders );

    gpu.draw( mesh );

    for ( int i = 0; i <= 6; i++ )
        gpu.unbind_shader_view_for_pixel_shader( i );
}
