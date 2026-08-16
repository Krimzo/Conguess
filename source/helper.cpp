#include "helper.h"


void load_shaders( kl::GPU& gpu, std::string_view const& name, kl::Shaders& out_shaders )
{
    const std::string full_path = kl::format( "shaders/", name, ".hlsl" );
    log( "Loading shaders ", full_path );
    const std::string shader_source = kl::read_file_string( full_path );
    if ( shader_source.empty() )
    {
        log_error( "Failed to load shaders ", full_path );
        return;
    }
    log( "Compiling shaders ", name );
    std::string out_vs_error, out_ps_error;
    out_shaders = gpu.create_shaders( shader_source, {}, &out_vs_error, &out_ps_error );
    if ( !out_shaders )
    {
        log_error( "Failed to compile shaders ", name );
        if ( !out_vs_error.empty() )
            log_error( "Vertex Shader Error: ", out_vs_error );
        if ( !out_ps_error.empty() )
            log_error( "Pixel Shader Error: ", out_ps_error );
        return;
    }
    log_success( "Shaders ", name, " are good." );
}

void load_mesh( kl::GPU& gpu, std::string_view const& name, kl::dx::Buffer& out_mesh )
{
    const std::string full_path = kl::format( "meshes/", name, ".obj" );
    log( "Loading mesh ", full_path );
    out_mesh = gpu.create_vertex_buffer( full_path, true );
    if ( !out_mesh )
    {
        log_error( "Failed to load mesh ", full_path );
        return;
    }
    log_success( "Mesh ", name, " is good." );
}

void load_texture( kl::GPU& gpu, std::string_view const& name, kl::dx::ShaderView& out_texture_sv )
{
    std::string full_path = kl::format( "textures/", name, ".png" );
    if ( !std::filesystem::exists( full_path ) )
        full_path = kl::format( "textures/", name, ".jpg" );
    log( "Loading image ", full_path );
    const kl::Image image{ full_path };
    if ( image.pixel_count() == 0 )
    {
        log_error( "Failed to load image ", full_path );
        return;
    }
    log( "Loading texture ", name );
    const kl::dx::Texture texture = gpu.create_texture( image );
    if ( !texture )
    {
        log_error( "Failed to load texture ", name );
        return;
    }
    log( "Creating shader view ", name );
    out_texture_sv = gpu.create_shader_view( texture, {} );
    if ( !out_texture_sv )
    {
        log_error( "Failed to create shader view ", name );
        return;
    }
    log_success( "Texture ", name, " is good." );
}
