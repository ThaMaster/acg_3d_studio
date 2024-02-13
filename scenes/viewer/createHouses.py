import sys
import random
import math

if len(sys.argv) < 2:
    print("Usage: " + sys.argv[0] + " <outfile> <numHouses>")
    sys.exit(1)

filename = sys.argv[1]
numHouses = 1

if len(sys.argv) > 1:
    numHouses = int(sys.argv[2])

print(numHouses)

file = open(filename, "w")

file.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<scene>\n")

def rand():
    return random.random() * 2 - 1

for i in range(0, numHouses):
    n = math.floor(random.random() * 10) + 1
    house = f'{n:03}' + ".fbx"

    pos = [str(rand() * 500), str(rand() * 500)]
    print(pos)
    ori = str(rand() * 180)

    file.write("  <node name=\"house"+str(i)+"\">\n")
    file.write("    <file path=\"models/ResidentialBuildings/ResidentialBuildings"+house+"\""+"> </file>\n")
    file.write("    <transform translate=\""+ pos[0] + " 0 " + pos[1] + "\"" + " rotate=\"0 " + ori + " 0\" scale=\"0.03 0.03 0.03\"></transform>\n")
    file.write("  </node>\n")


file.write("  <node name=\"ground\">\n")
file.write("    <file path=\"models/box.obj\"> </file>\n")
file.write("    <transform translate=\"0 0 0 \" rotate=\"0 0 0\" scale=\"1000 0.1 1000\"></transform>\n")
file.write("  </node>\n")


file.write("</scene>\n")



file.close()