#include "helper.h"


void load_shaders( kl::GPU& gpu, std::string_view const& name, kl::Shaders& out_shaders )
{
    const std::string full_path = kl::format( "shaders/", name, ".hlsl" );
    log( "Loading shaders ", full_path );
    const std::string shader_source = kl::read_file_string( full_path );
    if ( shader_source.empty() )
    {
        log( kl::colors::RED, "Failed to load shaders ", full_path, kl::colors::CONSOLE );
        return;
    }
    log( "Compiling shaders ", name );
    out_shaders = gpu.create_shaders( shader_source );
    if ( !out_shaders )
    {
        log( kl::colors::RED, "Failed to compile shaders ", name, kl::colors::CONSOLE );
        return;
    }
    log( kl::colors::GREEN, "Shaders ", name, " are good.", kl::colors::CONSOLE );
}

void load_mesh( kl::GPU& gpu, std::string_view const& name, kl::dx::Buffer& out_mesh )
{
    const std::string full_path = kl::format( "meshes/", name, ".obj" );
    log( "Loading mesh ", full_path );
    out_mesh = gpu.create_vertex_buffer( full_path, true );
    if ( !out_mesh )
    {
        log( kl::colors::RED, "Failed to load mesh ", full_path, kl::colors::CONSOLE );
        return;
    }
    log( kl::colors::GREEN, "Mesh ", name, " is good.", kl::colors::CONSOLE );
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
        log( kl::colors::RED, "Failed to load image ", full_path, kl::colors::CONSOLE );
        return;
    }
    log( "Loading texture ", name );
    const kl::dx::Texture texture = gpu.create_texture( image );
    if ( !texture )
    {
        log( kl::colors::RED, "Failed to load texture ", name, kl::colors::CONSOLE );
        return;
    }
    log( "Creating shader view ", name );
    out_texture_sv = gpu.create_shader_view( texture, {} );
    if ( !out_texture_sv )
    {
        log( kl::colors::RED, "Failed to create shader view ", name, kl::colors::CONSOLE );
        return;
    }
    log( kl::colors::GREEN, "Texture ", name, " is good.", kl::colors::CONSOLE );
}
