#include "conguess.h"


Conguess::Conguess()
{
    window.set_icon( "textures/icon.ico" );
    window.on_resize.emplace_back( [&]( kl::Int2 size )
        {
            gpu.resize_internal( size );
            gpu.set_viewport_size( size );
            camera.update_aspect_ratio( size );
            this->resize( size );
        } );
    window.resize( kl::SCREEN_SIZE / 2 );
    log( "Done." );
    game.reset();
}

bool Conguess::update()
{
    timer.update();
    input.update();
    gpu.clear_target_view( render_target_view );
    gpu.clear_target_view( info_target_view );
    gpu.clear_internal();
    skybox.update();
    earth.update();
    postprocess.update();
    gpu.swap_buffers( true );
    return window.process();
}

void Conguess::resize( kl::Int2 size )
{
    gpu.bind_internal_views();
    gpu.unbind_shader_view_for_pixel_shader( 0 );
    gpu.unbind_shader_view_for_pixel_shader( 1 );

    kl::dx::TextureDescriptor render_texture_desc{};
    render_texture_desc.Width = size.x;
    render_texture_desc.Height = size.y;
    render_texture_desc.MipLevels = 1;
    render_texture_desc.ArraySize = 1;
    render_texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    render_texture_desc.SampleDesc.Count = 1;
    render_texture_desc.Usage = D3D11_USAGE_DEFAULT;
    render_texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    kl::dx::Texture render_texture = gpu.create_texture( &render_texture_desc, {} );
    render_target_view = gpu.create_target_view( render_texture, {} );
    render_shader_view = gpu.create_shader_view( render_texture, {} );

    kl::dx::Texture index_texture = gpu.create_texture( &render_texture_desc, {} );
    info_target_view = gpu.create_target_view( index_texture, {} );
    info_shader_view = gpu.create_shader_view( index_texture, {} );
}
