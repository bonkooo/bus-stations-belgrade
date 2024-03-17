import math

output_filename, direction = input().split(" ")

try:
    plat, plon, rad = [float(i) for i in input().split(" ")]
    rad = rad / 1000
except:
    print("PODACI GRESKA")

buses = [i for i in input().split(" ")]

def process(filename):
    try:
        with open(filename, 'r') as file:
            data = [[i for i in line.strip().split("!")] for line in file.readlines()]
            return data
    except:
        print("DAT_GRESKA")


def distance(lat1, lon1, lat2, lon2):
    radconst = math.pi / 180

    lat1 = float(lat1) * radconst
    lon1 = float(lon1) * radconst
    lat2 = float(lat2) * radconst
    lon2 = float(lon2) * radconst

    r = 6371

    t1 = math.sin((lat1 - lat2) / 2) ** 2
    t2 = math.sin((lon1 - lon2) / 2) ** 2

    d = 2 * r * math.asin(math.sqrt(t1 + t2 * math.cos(lat1) * math.cos(lat2)))

    return d


# inputdata mi je 2d lista podataka za koju treba da se odredi ime
def obradi(input_data, plat, plon, rad):
    res = []
    for station in input_data:
        if distance(plat, plon, station[2], station[3]) <= rad:
            res.append(station)
    return res


reslist = []
res = []

for bus in buses:
    input_data = process(bus + "_" + direction + ".txt")
    station_list = obradi(input_data, plat, plon, rad)
    reslist.append(station_list)

for l in reslist:
    for station in l:
        if station not in res:
            res.append(station)

res.sort(key=lambda station: distance(plat, plon, station[2], station[3]))

strings = []
for i in res:
    strings.append('!'.join(i))


def ispisi(output_filename, station_list):
    try:
        with open(output_filename, 'w') as file:
            for station in station_list:
                station_data = station.split("!")
                station_data[2] = "{:.6f}".format(float(station_data[2]))
                station_data[3] = "{:.6f}".format(float(station_data[3]))
                file.write("!".join(station_data) + '\n')
            return station_list
    except:
        print("GRESKA")


ispisi(output_filename, strings)