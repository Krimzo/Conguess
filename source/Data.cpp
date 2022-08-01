#include "Data.h"
#include "Game.h"


// Parse
void ReadCountryData(const std::string& filePath) {
	Game::Log("Reading country data");
	std::ifstream file(filePath);
	kl::console::error(!file.is_open(), "Failed to open file \"" + filePath + "\"");

	std::stringstream ss;
	bool readingName = true;
	Data::Country country;
	Data::Polygon countryPolygon;
	kl::float2 polygonCoord;

	for (std::string line; std::getline(file, line);) {
		for (auto& c : line) {
			switch (c) {
				case '{':
					readingName = false;
					country.name = ss.str();
					ss = {};
					break;
				case '}':
					readingName = true;
					Data::countries.push_back(country);
					country = {};
					ss = {};
					break;

				case '[':
					countryPolygon = {};
					break;
				case ']':
					country.polygons.push_back(countryPolygon);
					break;

				case '(':
					polygonCoord = {};
					ss = {};
					break;
				case ',':
					polygonCoord.x = std::stof(ss.str());
					ss = {};
					break;
				case ')':
					polygonCoord.y = std::stof(ss.str());
					countryPolygon.coords.push_back(polygonCoord);
					break;

				default:
					ss << c;
					break;
			}
		}
	}
	file.close();
}

// Boundaries
kl::int2 CoordsToPoint(const kl::uint2& imageSize, const kl::float2& coords) {
	kl::int2 res;
	res.x = int(((coords.y + 180.0f) / 360.0f) * imageSize.x);
	res.y = imageSize.y - int(((coords.x + 90.0f) / 180.0f) * imageSize.y);
	return res;
}
kl::float2 PointToCoords(const kl::uint2& imageSize, const kl::int2& point) {
	kl::float2 res;
	res.x = ((imageSize.y - point.y) / float(imageSize.y)) * 180.0f - 90.0f;
	res.y = (point.x / float(imageSize.x)) * 360.0f - 180.0f;
	return res;
}
void DrawCountryBoundaries(kl::image& image, const std::vector<Data::Polygon>& polygons) {
	for (auto& polygon : polygons) {
		kl::float2 lastCoord = polygon.coords.back();
		for (auto& coord : polygon.coords) {
			const kl::int2 startPos = CoordsToPoint(image.size(), lastCoord);
			const kl::int2 endPos = CoordsToPoint(image.size(), coord);
			for (int i = 0; i < 2; i++) {
				image.drawLine(startPos + kl::int2(i, i), endPos + kl::int2(i, i), kl::colors::white);
			}
			lastCoord = coord;
		}
	}
}
void GenerateBoundaryMap(const std::string& filePath) {
	kl::image image(kl::uint2(8192, 4096));
	for (int i = 0; i < Data::countries.size(); i++) {
		kl::print("Generating boundary map ", i + 1, "/", Data::countries.size(), " (", Data::countries[i].name, ")");
		DrawCountryBoundaries(image, Data::countries[i].polygons);
	}
	image.toFile(filePath);
	kl::print("Done generating boundaries map");
}

// Indicies
kl::float4 MinMaxCoords(const Data::Polygon& polygon) {
	kl::float4 res = { -1e3f, 1e3f, 1e3f, -1e3f };
	for (auto& coord : polygon.coords) {
		res.x = max(res.x, coord.x);
		res.y = min(res.y, coord.y);
		res.z = min(res.z, coord.x);
		res.w = max(res.w, coord.y);
	}
	return res;
}
kl::color IntTo4ValueColor(int value) {
	int res[4] = {};
	for (int i = 0; i < 4; i++) {
		res[3 - i] = value % 4;
		value /= 4;
	}
	return kl::color(res[0] * 85, res[1] * 85, res[2] * 85, res[3] * 85);
}
void DrawCountryIndicies(kl::image& image, const std::vector<Data::Polygon>& polygons, int index) {
	for (auto& polygon : polygons) {
		const kl::float4 squareBounds = MinMaxCoords(polygon);
		const kl::uint2 topLeft = CoordsToPoint(image.size(), kl::float2(squareBounds.x, squareBounds.y));
		const kl::uint2 bottomRight = CoordsToPoint(image.size(), kl::float2(squareBounds.z, squareBounds.w));
		for (kl::uint2 point = topLeft; point.y <= bottomRight.y; point.y++) {
			for (point.x = topLeft.x; point.x <= bottomRight.x; point.x++) {
				if (polygon.contains(PointToCoords(image.size(), point))) {
					image.pixel(point, IntTo4ValueColor(index));
				}
			}
		}
	}
}
void GenerateIndiciesMap(const std::string& filePath) {
	kl::image image(kl::uint2(8192, 4096), kl::color(0, 0, 0, 0));
	std::atomic<int> mapCounter = 0;
	kl::async::loop(0, Data::countries.size(), [&](uint t, int64 i) {
		kl::print("Generating index map ", ++mapCounter, "/", Data::countries.size(), " (", Data::countries[i].name, ")");
		DrawCountryIndicies(image, Data::countries[i].polygons, int(i + 1));
	});
	image.toFile(filePath);
	kl::print("Done generating indicies map");
}

// Init
void Data::Initialize() {
	ReadCountryData("resource/data/countries.txt");

	const std::string boundariesFile = "resource/textures/earth_boundaries.png";
	if (!std::filesystem::exists(boundariesFile)) {
		GenerateBoundaryMap(boundariesFile);
	}

	const std::string indiciesFile = "resource/textures/earth_indicies.png";
	if (!std::filesystem::exists(indiciesFile)) {
		GenerateIndiciesMap(indiciesFile);
	}
}
