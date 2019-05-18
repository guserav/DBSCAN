import matplotlib.pyplot as plt
import numpy as np
import re
import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument("input", nargs='?', type=argparse.FileType('r'), default=sys.stdin)
parser.add_argument("output", nargs='?', type=str, default="plot.png")
parser.add_argument("-d", "--delim", nargs='?', help="Delimiter to use for splitting point coordinates", default=";")
parser.add_argument("-r", "--split", nargs='?', help="(Regex) to use for splitting several points an depth", default=",|:")
parser.add_argument("-s", "--strip", nargs='?', help="Strip these characters away from points on both sides", default="[]\n")
parser.add_argument("--visualizeDepth", nargs='?', help="Visualize the following depths", default=">1")
parser.add_argument("--dpi", type=int, default=2400)
args = parser.parse_args()

colors = ['b', 'g', 'r', 'c', 'm', 'y']
reg = re.compile(args.split)
x = []
y = []
plt.figure(dpi=300)
for line in list(args.input)[::1]:
    lineSplit = reg.split(line)
    if(len(lineSplit) == 2):
        split = lineSplit[1].strip(args.strip).split(args.delim)
        #if(float(split[0]) < 200 and float(split[0]) > 100 and float(split[1]) < 200 and float(split[1]) > 100):
        x.append(float(split[0]))
        y.append(float(split[1]))
    else:
        split1 = lineSplit[1].strip(args.strip).split(args.delim)
        split2 = lineSplit[2].strip(args.strip).split(args.delim)
        split1 = list(map(float, split1))
        split2 = list(map(float, split2))
        a = [split1[0],
            split2[0],
            split2[0],
            split1[0],
            split1[0]]
        b = [split1[1],
            split1[1],
            split2[1],
            split2[1],
            split1[1]]
        depth = int(lineSplit[0])
        if args.visualizeDepth[0] == '>':
            if depth > int(args.visualizeDepth[1:]):
                plt.plot(a, b, colors[depth], linewidth=1/(2*depth+1))
        elif args.visualizeDepth[0] == '<':
            if depth < int(args.visualizeDepth[1:]):
                plt.plot(a, b, colors[depth], linewidth=1/(2*depth+1))
plt.plot(x, y, 'k.-',linewidth=0, markersize=1/5, markeredgewidth=1/5)
plt.savefig('plot.png', dpi=args.dpi)
