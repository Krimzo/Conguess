#include "conguess.h"


ConguessEarth::ConguessEarth( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    depth_state = gpu.create_depth_state( true, false, false );

    load_shaders( gpu, "earth", shaders );
    load_mesh( gpu, "sphere", mesh );
    load_texture( gpu, "earth_day", earth_day_sv );
    load_texture( gpu, "earth_night", earth_night_sv );
    load_texture( gpu, "earth_clouds", earth_clouds_sv );
    load_texture( gpu, "earth_normal", earth_normal_sv );
    load_texture( gpu, "earth_roughness", earth_roughness_sv );
    load_texture( gpu, "earth_boundaries", earth_boundaries_sv );
    load_texture( gpu, "earth_indices", earth_indices_sv );
}

void ConguessEarth::update()
{
    auto& gpu = conguess.gpu;

    gpu.bind_target_depth_views( { conguess.render_target_view.get(), conguess.index_target_view.get() },
        gpu.back_depth_view() );
    gpu.bind_depth_state( depth_state );

    gpu.bind_shader_view_for_pixel_shader( earth_day_sv, 0 );
    gpu.bind_shader_view_for_pixel_shader( earth_night_sv, 1 );
    gpu.bind_shader_view_for_pixel_shader( earth_clouds_sv, 2 );
    gpu.bind_shader_view_for_pixel_shader( earth_normal_sv, 3 );
    gpu.bind_shader_view_for_pixel_shader( earth_roughness_sv, 4 );
    gpu.bind_shader_view_for_pixel_shader( earth_boundaries_sv, 5 );
    gpu.bind_shader_view_for_pixel_shader( earth_indices_sv, 6 );

    struct alignas( 16 ) CB
    {
        kl::Float4x4 W;
        kl::Float4x4 VP;
        kl::Float4 SUN_DIRECTION;
        kl::Float4 CAMERA_POSITION;
        kl::Float4 MISC_DATA;
    } cb = {};

    cb.W = kl::Float4x4::rotation( conguess.sphere_rotation );
    cb.VP = conguess.camera.matrix();
    cb.SUN_DIRECTION.xyz() = kl::normalize( conguess.sun_direction );
    cb.CAMERA_POSITION.xyz() = conguess.camera.position;
    cb.MISC_DATA = {
        conguess.timer.elapsed() / 1800.0f,
        float( conguess.input.mouse_country_index + 1 ),
        (float) render_clouds,
        0.0f
    };

    shaders.upload( cb );
    gpu.bind_shaders( shaders );

    gpu.draw( mesh );

    for ( int i = 0; i <= 6; i++ )
        gpu.unbind_shader_view_for_pixel_shader( i );
}
