import json
import requests
import bs4 as bs


def get_wanted_countries(min_size):
    original_html = requests.get(
        "https://www.worldometers.info/geography/alphabetical-list-of-countries/"
    ).text
    parsed_html = bs.BeautifulSoup(original_html, features="html.parser")
    table_class = parsed_html.find("div", {"class": "table-responsive"})
    country_parts = table_class.find("tbody").findChildren("td")
    wanted = []
    country = []
    for i in range(len(country_parts)):
        modulo = i % 5
        if modulo == 0:
            if len(country) == 2 and country[1] > min_size:
                wanted.append(country[0].lower())
            country = []
        elif modulo == 1:
            country.append(country_parts[i].text)
        elif modulo == 3:
            country.append(
                int("".join(c for c in country_parts[i].text if c.isdigit()))
            )
    return wanted


def write_coords(file, coords):
    file.write("[")
    for coord in coords:
        file.write("(" + str(coord[1]) + "," + str(coord[0]) + ")")
    file.write("]")


wanted_countries = get_wanted_countries(30000)

file = open("countries.json")
full_data = json.load(file)
file.close()

file = open("countries.txt", "w")
for feature in full_data["features"]:
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
