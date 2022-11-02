#include "data.h"

#include "game.h"


static void read_country_data(const std::string& filepath)
{
	game::log("Reading country data");
	std::ifstream file(filepath);
	kl::assert(!file.is_open(), "Failed to open file \"" + filepath + "\"");

	std::stringstream ss = {};
	country country = {};
	polygon country_polygon = {};
	kl::float2 polygon_coord = {};

	for (std::string line; std::getline(file, line);) {
		for (auto& c : line) {
			bool reading_name = true;
			switch (c) {
				case '{':
					reading_name = false;
					country.name = ss.str();
					ss = {};
					break;
				case '}':
					reading_name = true;
					data::countries.push_back(country);
					country = {};
					ss = {};
					break;

				case '[':
					country_polygon = {};
					break;
				case ']':
					country.polygons.push_back(country_polygon);
					break;

				case '(':
					polygon_coord = {};
					ss = {};
					break;
				case ',':
					polygon_coord.x = std::stof(ss.str());
					ss = {};
					break;
				case ')':
					polygon_coord.y = std::stof(ss.str());
					country_polygon.coords.push_back(polygon_coord);
					break;

				default:
					ss << c;
					break;
			}
		}
	}
	file.close();
}

static kl::int2 coords_to_point(const kl::uint2& image_size, const kl::float2& coords)
{
	kl::int2 res;
	res.x = static_cast<int>(((coords.y + 180.0f) / 360.0f) * image_size.x);
	res.y = image_size.y - static_cast<int>(((coords.x + 90.0f) / 180.0f) * image_size.y);
	return res;
}

static kl::float2 point_to_coords(const kl::uint2& image_size, const kl::int2& point)
{
	kl::float2 res;
	res.x = ((image_size.y - point.y) / static_cast<float>(image_size.y)) * 180.0f - 90.0f;
	res.y = (point.x / static_cast<float>(image_size.x)) * 360.0f - 180.0f;
	return res;
}

static void draw_country_boundaries(kl::image& image, const std::vector<polygon>& polygons)
{
	for (const auto& [coords] : polygons) {
		kl::float2 last_coord = coords.back();
		for (auto& coord : coords) {
			const kl::int2 start_pos = coords_to_point(image.size(), last_coord);
			const kl::int2 end_pos = coords_to_point(image.size(), coord);
			for (int i = 0; i < 2; i++) {
				image.draw_line(start_pos + kl::int2(i, i), end_pos + kl::int2(i, i), kl::colors::white);
			}
			last_coord = coord;
		}
	}
}

static void generate_boundary_map(const std::string& filepath)
{
	kl::image image(kl::uint2(8192, 4096));
	for (uint64_t i = 0; i < data::countries.size(); i++) {
		kl::print("Generating boundary map ", i + 1, "/", data::countries.size(), " (", data::countries[i].name, ")");
		draw_country_boundaries(image, data::countries[i].polygons);
	}
	bool ignored = image.save_to_file(filepath);
	kl::print("Done generating boundaries map");
}

static kl::float4 min_max_coords(const polygon& polygon)
{
	kl::float4 res = { -1e3f, 1e3f, 1e3f, -1e3f };
	for (auto& coord : polygon.coords) {
		res.x = std::max(res.x, coord.x);
		res.y = std::min(res.y, coord.y);
		res.z = std::min(res.z, coord.x);
		res.w = std::max(res.w, coord.y);
	}
	return res;
}

static kl::color int_to4_value_color(int value)
{
	int result[4] = {};
	for (int i = 0; i < 4; i++) {
		result[3 - i] = value % 4;
		value /= 4;
	}
	return {
		static_cast<uint8_t>(result[0] * 85),
		static_cast<uint8_t>(result[1] * 85),
		static_cast<uint8_t>(result[2] * 85),
		static_cast<uint8_t>(result[3] * 85),
	};
}

static void draw_country_indices(kl::image& image, const std::vector<polygon>& polygons, int index)
{
	for (auto& polygon : polygons) {
		const kl::float4 square_bounds = min_max_coords(polygon);
		const kl::int2 top_left = coords_to_point(image.size(), kl::float2(square_bounds.x, square_bounds.y));
		const kl::int2 bottom_right = coords_to_point(image.size(), kl::float2(square_bounds.z, square_bounds.w));
		for (kl::int2 point = top_left; point.y <= bottom_right.y; point.y++) {
			for (point.x = top_left.x; point.x <= bottom_right.x; point.x++) {
				if (polygon.contains(point_to_coords(image.size(), point))) {
					image.set_pixel(kl::uint2(point), int_to4_value_color(index));
				}
			}
		}
	}
}

static void generate_indices_map(const std::string& filepath)
{
	std::atomic map_counter = 0;
	const auto countries_count = static_cast<int64_t>(data::countries.size());
	kl::image image(kl::uint2(8192, 4096), kl::color(0, 0, 0, 0));
	kl::async::loop(0, countries_count, [&](uint32_t, const int64_t i) {
		draw_country_indices(image, data::countries[i].polygons, static_cast<int>(i + 1));
		kl::print("Generated index map ", ++map_counter, "/", countries_count, " (", data::countries[i].name, ")");
	});
	bool ignored = image.save_to_file(filepath);
	kl::print("Generated indices map");
}

void data::initialize()
{
	read_country_data("resource/data/countries.txt");

	if (const std::string boundaries_file = "resource/textures/earth_boundaries.png"; !std::filesystem::exists(boundaries_file)) {
		generate_boundary_map(boundaries_file);
	}

	if (const std::string indices_file = "resource/textures/earth_indices.png"; !std::filesystem::exists(indices_file)) {
		generate_indices_map(indices_file);
	}
}
