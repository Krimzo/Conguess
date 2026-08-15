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
    gpu.bind_shader_view_for_pixel_shader( conguess.info_shader_view, 1 );

    struct alignas( 16 ) CB
    {
        float RENDER_BOUNDS;
        float MOUSE_LMB;
    } cb = {};

    cb.RENDER_BOUNDS = render_bounds;
    cb.MOUSE_LMB = conguess.window.mouse.left;

    shaders.upload( cb );
    gpu.bind_shaders( shaders );

    gpu.draw( mesh );
}
