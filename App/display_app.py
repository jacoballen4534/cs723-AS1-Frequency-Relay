import sys
import matplotlib.pyplot as plt

# plt.style.use('fivethirtyeight')


def main():
    print("ENTERED PYTHON APP\r\n")
    frequencies = []
    rocs = []
    timestamps = []
    x = []

    fig = plt.figure()

    freqPlot = fig.add_subplot(121)
    rocPlot = fig.add_subplot(122)
    fig.show()

    for line in sys.stdin:
        if ("SNAPSHOT:" in line):
            freqPlot.cla()
            rocPlot.cla()
            freqPlot.plot(x, frequencies, color='b')
            rocPlot.plot(x, rocs, color='r')
            fig.canvas.draw()

            frequencies = []
            rocs = []
            timestamps = []
            x = []
            readings = 0

        if ("Reading" in line):
            keys = line.split(',')
            if (len(keys) == 4 and keys[0] == "Reading"):
                frequencies.insert(0, float(keys[1]))
                rocs.insert(0, float(keys[2]))
                timestamps.insert(0, keys[3])
                x.append(readings)
                readings += 1


if __name__ == "__main__":
    main()
