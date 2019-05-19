from sklearn import datasets
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", nargs='?', type=argparse.FileType('w'), default=sys.stdin)
parser.add_argument("-s", "--size", nargs='1', type=int)
parser.add_argument("-n", "--noise", nargs='?', type=float, default=None)
parser.add_argument("dataSet", nargs='?', type=str, help="Possible values blobs, circles, moons" default="blobs")
args = parser.parse_args()

data = None

if args.dataset == "circles":
    data = datasets.make_circles(n_samples=args.size, factor=.4, noise=args.noise)
elif args.dataset == "moons":
    data = datasets.make_moons(n_samples=args.size, noise=args.noise)
elif args.dataset == "blobs":
    data = datasets.make_blobs(n_samples=args.size, noise=args.noise)

else:
    print("Unknown dataset")
    exit(-1)

