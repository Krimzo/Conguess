#include "Data.h"
#include "Game.h"


void ReadCountryData() {
	static const std::string dataFilePath = "resource/data/countries.txt";
	Game::Log("Reading country data");
	std::ifstream file(dataFilePath);
	kl::console::error(!file.is_open(), "Failed to open file \"" + dataFilePath + "\"");

	std::stringstream ss;
	bool readingName = true;
	Data::Country buildingCountry;
	Data::Polygon buildingPolygon;
	kl::float2 buildingCoord;

	for (std::string line; std::getline(file, line);) {
		for (auto& c : line) {
			switch (c) {
			case '{':
				readingName = false;
				buildingCountry.name = ss.str();
				ss = {};
				break;
			case '}':
				readingName = true;
				Data::countries.push_back(buildingCountry);
				buildingCountry = {};
				ss = {};
				break;

			case '[':
				buildingPolygon = {};
				break;
			case ']':
				buildingCountry.polygons.push_back(buildingPolygon);
				break;

			case '(':
				buildingCoord = {};
				ss = {};
				break;
			case ',':
				buildingCoord.x = std::stof(ss.str());
				ss = {};
				break;
			case ')':
				buildingCoord.y = std::stof(ss.str());
				buildingPolygon.coords.push_back(buildingCoord);
				break;

			default:
				ss << c;
				break;
			}
		}
	}

	file.close();
}

kl::int2 ConvertCoords(const kl::uint2& imageSize, const kl::float2& coords) {
	kl::int2 res;
	res.x = int(((coords.y + 180.0f) / 360.0f) * imageSize.x);
	res.y = imageSize.y - int(((coords.x + 90.0f) / 180) * imageSize.y);
	return res;
}

void GenerateBoundaryMap() {
	Game::Log("Generating boundary map");
	kl::image image(kl::uint2(8192, 4096));
	for (auto& country : Data::countries) {
		for (auto& polygon : country.polygons) {
			kl::float2 lastCoord = polygon.coords.back();
			for (auto& coord : polygon.coords) {
				const kl::int2 startPos = ConvertCoords(image.size(), lastCoord);
				const kl::int2 endPos = ConvertCoords(image.size(), coord);
				for (int i = 0; i < 2; i++) {
					image.drawLine(startPos + i, endPos + i, kl::colors::white);
				}
				lastCoord = coord;
			}
		}
	}
	image.toFile("resource/textures/earth_boundaries.png");
}

void Data::Initialize() {
	ReadCountryData();
	GenerateBoundaryMap();
}
