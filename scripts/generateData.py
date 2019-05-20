#! /usr/bin/env nix-shell
#! nix-shell -i python3 -p python3 python3Packages.scikitlearn
from sklearn import datasets
import argparse
import sys
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", nargs='?', type=argparse.FileType('w'), default=sys.stdout)
parser.add_argument("-s", "--size", nargs='?', type=int, required=True)
parser.add_argument("-n", "--noise", nargs='?', type=float)
parser.add_argument("-d", "--delim", nargs='?', default=";")
parser.add_argument("-r", "--randomState", nargs='?', type=int, default=42)
parser.add_argument("--dimensions", nargs='?', type=int, default=2)
parser.add_argument("--clusters", nargs='?', type=int, default=3)
parser.add_argument("dataSet", nargs='?', type=str, help="Possible values blobs, circles, moons, noise", default="blobs")
args = parser.parse_args()

data = None

offset = 0
factor = 1
if args.dataSet == "circles":
    data = datasets.make_circles(n_samples=args.size, factor=.4, noise=args.noise, random_state=args.randomState)[0]
    offset = 50
    factor = 20
elif args.dataSet == "noise":
    data = np.random.rand(args.size,args.dimensions)
    factor = 50
elif args.dataSet == "moons":
    data = datasets.make_moons(n_samples=args.size, noise=args.noise, random_state=args.randomState)[0]
    offset = 50
    factor = 20
elif args.dataSet == "blobs":
    data = datasets.make_blobs(n_samples=args.size, n_features=args.dimensions, centers=args.clusters, random_state=args.randomState, center_box=(10, 90.0))[0]

else:
    print("Unknown dataset")
    exit(-1)

try:
    for point in data:
        args.output.write(args.delim.join(str((item * factor) + offset) for item in point) + "\n")
finally:
    args.output.close()
