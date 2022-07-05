#include "Data.h"
#include "Game.h"


static const std::string dataFilePath = "resource/data/countries.txt";

void Data::Initialize() {
	std::ifstream file(dataFilePath);
	kl::console::error(!file.is_open(), "Failed to open file \"" + dataFilePath + "\"");

	std::stringstream ss;
	bool readingName = true;
	Country buildingCountry;
	Polygon buildingPolygon;
	kl::float2 buildingCoord;

	Game::Log("Reading country data");
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
				countries.push_back(buildingCountry);
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
