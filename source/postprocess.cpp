#include "conguess.h"


ConguessPostprocess::ConguessPostprocess( Conguess& conguess )
    : conguess( conguess )
{
    auto& gpu = conguess.gpu;

    mesh = gpu.create_screen_mesh();
    raster_state = gpu.create_raster_state( false, false );
    depth_state = gpu.create_depth_state( false, false, false );
    load_shaders( gpu, "postprocess", shaders );
    sampler = gpu.create_sampler_state( true, false );
}

void ConguessPostprocess::update()
{
    auto& gpu = conguess.gpu;

    gpu.bind_internal_views();
    gpu.bind_raster_state( raster_state );
    gpu.bind_depth_state( depth_state );

    gpu.bind_shader_view_for_pixel_shader( conguess.render_shader_view, 0 );
    gpu.bind_shader_view_for_pixel_shader( conguess.info_shader_view, 1 );
    gpu.bind_sampler_state_for_pixel_shader( sampler, 0 );

    struct alignas( 16 ) CB
    {
        kl::Float4 HIGHLIGHT_COUNTRY_COLOR_MULTI;
        kl::Float4 HOVER_COUNTRY_COLOR_MULTI;
        kl::Float4 HOLD_COUNTRY_COLOR_MULTI;
        kl::Float4 BORDER_COLOR;
        float RENDER_BORDERS;
        float RENDER_ATMOSPHERE;
        float MOUSE_LMB;
    } cb = {};

    cb.HIGHLIGHT_COUNTRY_COLOR_MULTI = highlight_country_color_multi;
    cb.HOVER_COUNTRY_COLOR_MULTI = hover_country_color_multi;
    cb.HOLD_COUNTRY_COLOR_MULTI = hold_country_color_multi;
    cb.BORDER_COLOR = border_color;
    cb.RENDER_BORDERS = render_borders;
    cb.RENDER_ATMOSPHERE = conguess.game.should_highlight() ? false : render_atmosphere;
    cb.MOUSE_LMB = conguess.window.mouse.left;

    shaders.upload( cb );
    gpu.bind_shaders( shaders );

    gpu.draw( mesh );
}
