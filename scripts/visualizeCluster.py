import matplotlib.pyplot as plt
import numpy as np
import re
import argparse
import sys
import random

parser = argparse.ArgumentParser()
parser.add_argument("-i", "--input", nargs='?', type=argparse.FileType('r'), default=sys.stdin)
parser.add_argument("-o", "--output", nargs='?', type=str, default="plot.png")
parser.add_argument("-d", "--delim", nargs='?', help="Delimiter to use for splitting point coordinates", default=";")
parser.add_argument("-s", "--strip", nargs='?', help="Strip these characters away from points on both sides", default="[]\n")
parser.add_argument("--dpi", type=int, default=2400)
args = parser.parse_args()

colors = ["black", (0.5, 0, 0), (0, 0.5, 0), (0, 0, 0.5), (0.5, 0.5, 0), (0.5, 0, 0.5), (0, 0.5, 0.5),
        (0, 0.75, 0.25), (0, 0.25, 0.75), (0.25, 0.75, 0), (0.75, 0.25, 0), (0.75, 0, 0.25), (0.25, 0, 0.75), ]
def getColor(cluster):
    if cluster < len(colors):
        return colors[cluster]
    return (r(), r(), r())
r = lambda: random.random() / 2

clusterDict={}
for line in args.input:
    split = line.strip(args.strip).split(args.delim)
    data = None
    if int(split[2]) in clusterDict:
        data = clusterDict[int(split[2])]
    else:
        data = {"x":[], "y":[]}
    data["x"].append(float(split[0]))
    data["y"].append(float(split[1]))
    clusterDict[int(split[2])] = data
for cluster, data in clusterDict.items():
    plt.scatter(data["x"], data["y"], marker='.', c=getColor(cluster))
    #plt.plot(data["x"], data["y"], linewidth=0, markersize=1/5, markeredgewidth=1/5, c=(0, 0, 0))
plt.savefig(args.output, dpi=args.dpi)
