#include "conguess.h"


ConguessPostprocess::ConguessPostprocess( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    depth_state = gpu.create_depth_state( false, false, false );

    log( "Compiling postprocess shaders" );
    shaders = gpu.create_shaders( kl::read_file_string( "shaders/postprocess.hlsl" ) );

    log( "Loading screen mesh" );
    mesh = gpu.create_screen_mesh();
}

void ConguessPostprocess::update()
{
    auto& gpu = conguess.gpu;

    gpu.bind_internal_views();
    gpu.bind_depth_state( depth_state );
    gpu.bind_shaders( shaders );

    gpu.bind_shader_view_for_pixel_shader( conguess.render_shader_view, 0 );
    gpu.bind_shader_view_for_pixel_shader( conguess.index_shader_view, 1 );

    const kl::Float4 misc_data = {
        (float) render_bounds,
        (float) conguess.window.mouse.left,
        0.0f,
        0.0f
    };
    shaders.upload( misc_data );

    gpu.draw( mesh );
}
