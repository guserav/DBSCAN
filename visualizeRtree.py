import matplotlib.pyplot as plt
import numpy as np
import re

file = open("../output.data", "r")
colors = ['b', 'g', 'r', 'c', 'm', 'y']
reg = re.compile(",|:")
x = []
y = []
plt.figure(dpi=300)
for line in list(file)[::-1]:
    lineSplit = reg.split(line)
    if(len(lineSplit) == 2):
        split = lineSplit[1].strip("[]\n").split(";")
        x.append(float(split[0]))
        y.append(float(split[1]))
    else:
        split1 = lineSplit[1].strip("[]\n").split(";")
        split2 = lineSplit[2].strip("[]\n").split(";")
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
        if depth < 4:
            plt.plot(a, b, colors[depth], linewidth=1/(2*depth+1))
plt.show()
