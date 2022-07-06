import json


def write_coords(file, coords):
    file.write("[")
    for coord in coords:
        file.write("(" + str(coord[1]) + "," + str(coord[0]) + ")")
    file.write("]")

# Read
file = open("countries.json")
full_data = json.load(file)
file.close()

# Write
file = open("countries.txt", "w")
for feature in full_data["features"]:
    file.write(feature["properties"]["ADMIN"].replace('(', '').replace(')', ''))
    file.write("{")
    for polygon in feature["geometry"]["coordinates"]:
        if feature["geometry"]["type"].lower() == "multipolygon":
            for sub_polygon in polygon:
                write_coords(file, sub_polygon)
        else:
            write_coords(file, polygon)
    file.write("}\n")
file.close()
