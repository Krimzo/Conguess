#include "utility/file.h"
#include "utility/console.h"
#include "utility/strings.h"

#include <fstream>


std::string kl::files::get_extension(const std::string& filepath)
{
    return std::filesystem::path(filepath).extension().string();
}

std::vector<std::string> kl::files::get_files(const std::string& path, const bool recursive)
{
    std::vector<std::string> files;
    if (!recursive) {
        for (const auto& file : std::filesystem::directory_iterator(path)) {
            if (!file.is_directory()) {
                files.push_back(file.path().string());
            }
        }
    }
    else {
        for (const auto& file : std::filesystem::recursive_directory_iterator(path)) {
            if (!file.is_directory()) {
                files.push_back(file.path().string());
            }
        }
    }
    return files;
}

std::string kl::files::read_string(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::stringstream buffer;
    if (warning(!stream.is_open(), "Failed to open file \"" + filepath + "\"")) {
        return {};
    }

    buffer << stream.rdbuf();
    stream.close();
    return buffer.str();
}

bool kl::files::write_string(const std::string& filepath, const std::string& data)
{
    std::ofstream stream(filepath);
    if (warning(!stream.is_open(), "Failed to open file \"" + filepath + "\"")) {
        return false;
    }

    stream << data;
    stream.close();
    return true;
}

bool kl::files::append_string(const std::string& filepath, const std::string& data, int position)
{
    std::fstream stream(filepath, std::ios::in | std::ios::out);
    if (warning(!stream.is_open(), "Failed to open file \"" + filepath + "\"")) {
        return false;
    }

    if (position < 0) {
        stream.seekp(0, std::ios_base::end);
    }
    else {
        stream.seekp(position);
    }
    stream << data;

    stream.close();
    return true;
}

std::vector<kl::vertex> kl::files::parse_mesh(const std::string& filepath, const bool flip_z)
{
    std::ifstream file(filepath);
    if (warning(!file.is_open(), "Failed to open file \"" + filepath + "\"")) {
        return {};
    }
    
    std::vector<float3> xyz_data = {};
    std::vector<float2> uv_data = {};
    std::vector<float3> normal_data = {};
    std::vector<vertex> vertex_data = {};

    const float z_flip = flip_z ? -1.0f : 1.0f;
    for (std::string line; std::getline(file, line);) {
        if (const std::vector<std::string> parts = strings::split(line, ' '); parts[0] == "v") {
            xyz_data.emplace_back(std::stof(parts[1]), std::stof(parts[2]), std::stof(parts[3]) * z_flip);
        }
        else if (parts[0] == "vt") {
            uv_data.emplace_back(std::stof(parts[1]), std::stof(parts[2]));
        }
        else if (parts[0] == "vn") {
            normal_data.emplace_back(std::stof(parts[1]), std::stof(parts[2]), std::stof(parts[3]) * z_flip);
        }
        else if (parts[0] == "f") {
            for (int i = 1; i < 4; i++) {
                const std::vector<std::string> line_part_parts = strings::split(parts[i], '/');
                vertex_data.emplace_back(
                    xyz_data[std::stoull(line_part_parts[0]) - 1],
                    uv_data[std::stoull(line_part_parts[1]) - 1],
                    normal_data[std::stoull(line_part_parts[2]) - 1]
                );
            }
        }
    }
    
    file.close();
    return vertex_data;
}

kl::file::file()
{}

kl::file::file(const std::string& filepath, const bool clear)
{
    open(filepath, clear);
}

kl::file::~file()
{
    close();
}

kl::file::operator bool() const
{
    return is_open();
}

bool kl::file::is_open() const
{
    return static_cast<bool>(file_);
}

bool kl::file::open(const std::string& filepath, bool clear)
{
    close();
    return !warning(fopen_s(&file_, filepath.c_str(), clear ? "wb+" : "ab+"),
                    "Failed to open file \"" + filepath + "\"");
}

bool kl::file::close()
{
    if (file_) {
        const int state = fclose(file_);
        file_ = nullptr;
        return static_cast<bool>(state);
    }
    return false;
}

bool kl::file::seek(const int64_t position) const
{
    if (file_) {
        if (position >= 0) {
            return !fseek(file_, static_cast<long>(position), SEEK_SET);
        }
        return !fseek(file_, static_cast<long>(position + 1), SEEK_END);
    }
    return false;
}

bool kl::file::move(const int64_t delta) const
{
    if (file_) {
        return !fseek(file_, static_cast<long>(delta), SEEK_CUR);
    }
    return false;
}

bool kl::file::rewind() const
{
    if (file_) {
        return seek(0);
    }
    return false;
}

bool kl::file::unwind() const
{
    if (file_) {
        return seek(-1);
    }
    return false;
}

int64_t kl::file::tell() const
{
    if (file_) {
        return ftell(file_);
    }
    return -1;
}
