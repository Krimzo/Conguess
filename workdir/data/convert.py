import json
import requests
import bs4 as bs


def get_wanted_countries(min_area):
    html = requests.get("https://www.worldometers.info/geography/alphabetical-list-of-countries/").text
    html = bs.BeautifulSoup(html, features="html.parser")
    country_parts = html.find("table", {"class": "datatable"}).find("tbody").find_all("td")
    country = []
    wanted = []
    for i in range(len(country_parts)):
        match i % 5:
            case 0:
                if len(country) == 2:
                    country_str = country[0].lower().strip()
                    if country[1] > min_area:
                        wanted.append(country_str)
                    else:
                        print("ignoring: " + country_str + " [" + str(country[1]) + " < " + str(min_area) + "]")
                country = []
            case 1:
                country.append(country_parts[i].text)
            case 3:
                country.append(int("".join(c for c in country_parts[i].text if c.isdigit())))
    return wanted


def write_coords(file, coords):
    file.write("[")
    for coord in coords:
        file.write("(" + str(coord[1]) + "," + str(coord[0]) + ")")
    file.write("]")


if __name__ == "__main__":
    wanted_countries = get_wanted_countries(20000)
    file = open("countries.json")
    country_data = json.load(file)
    file.close()
    file = open("countries.txt", "w")
    for feature in country_data["features"]:
        country_name = feature["properties"]["ADMIN"].replace("(", "").replace(")", "")
        if country_name.lower() in wanted_countries:
            file.write(country_name)
            file.write("{")
            for polygon in feature["geometry"]["coordinates"]:
                if feature["geometry"]["type"].lower() == "multipolygon":
                    for sub_polygon in polygon:
                        write_coords(file, sub_polygon)
                else:
                    write_coords(file, polygon)
            file.write("}\n")
            print("Saved data for " + country_name)
    file.close()
