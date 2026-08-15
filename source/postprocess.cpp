#include "conguess.h"


ConguessPostprocess::ConguessPostprocess( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    depth_state = gpu.create_depth_state( false, false, false );

    load_shaders( gpu, "postprocess", shaders );
    mesh = gpu.create_screen_mesh();
}

void ConguessPostprocess::update()
{
    auto& gpu = conguess.gpu;

    gpu.bind_internal_views();
    gpu.bind_depth_state( depth_state );

    gpu.bind_shader_view_for_pixel_shader( conguess.render_shader_view, 0 );
    gpu.bind_shader_view_for_pixel_shader( conguess.index_shader_view, 1 );

    struct alignas( 16 ) CB
    {
        kl::Float4 misc_data{};
    } cb = {};

    cb.misc_data = {
        (float) render_bounds,
        (float) conguess.window.mouse.left,
        0.0f,
        0.0f
    };

    shaders.upload( cb );
    gpu.bind_shaders( shaders );

    gpu.draw( mesh );
}
