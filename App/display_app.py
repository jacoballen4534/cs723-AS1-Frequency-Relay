import sys
import PySimpleGUI as sg
import threading
import time
import copy 
import collections

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, FigureCanvasAgg
from matplotlib.figure import Figure

NUM_POINTS = 2000

freq_q = collections.deque([0]*NUM_POINTS,maxlen=NUM_POINTS)
roc_q = collections.deque([0]*NUM_POINTS,maxlen=NUM_POINTS)
ts_q = collections.deque([0]*NUM_POINTS,maxlen=NUM_POINTS)

data_lock = threading.RLock()


def draw_figure(canvas, figure, loc=(0, 0)):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg

class ReadInput(threading.Thread):
    def run(self):
        global freq_q, roc_q, ts_q

        for line in sys.stdin:
            if ("_fr," in line):
                keys = line.split(',')
                freq_q.append(float(keys[1]))
                roc_q.append(float(keys[2]))
                ts_q.append(float(keys[3]))
        
def main():
    global freq_q, roc_q, ts_q

    print("ENTERED PYTHON APP\r\n")
    
    ReadInput().start()

    # define the form layout
    layout = [[sg.Text('Animated Matplotlib', size=(40, 1),
                justification='center', font='Helvetica 20')],
              [sg.Canvas(size=(640, 480), key='-CANVAS-')],
              [sg.Button('Exit', size=(10, 1), pad=((280, 0), 3), font='Helvetica 14')]]

    # create the form and show it without the plot
    window = sg.Window('Demo Application - Embedding Matplotlib In PySimpleGUI',
                layout, finalize=True)

    canvas_elem = window['-CANVAS-']

    canvas = canvas_elem.TKCanvas

    # draw the initial plot in the window
    fig = Figure()
    ax = fig.add_subplot(111)
    ax.set_xlabel("X axis")
    ax.set_ylabel("Y axis")
    ax.set_ylim([45.0, 55.0])
    ax.grid()
    fig_agg = draw_figure(canvas, fig)

    while(True):
        event, values = window.read(timeout=10)
        if event in ('Exit', None):
            exit(69)
        ax.cla()                    # clear the subplot
        ax.grid()                   # draw the grid
        ax.plot(range(NUM_POINTS), list(freq_q),  color='purple')
        ax.set_ylim([45.0, 55.0])
        fig_agg.draw()

    window.close()
    


if __name__ == "__main__":
    main()