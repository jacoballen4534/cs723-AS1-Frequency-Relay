import sys
import PySimpleGUI as sg
import threading
import time
import copy
import collections

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, FigureCanvasAgg
from matplotlib.figure import Figure

NUM_POINTS = 2000
NUM_OF_SWITCHES = 10
NUM_OF_LOADS = 10

FSMDecoder = {
    '0': 'IDLE',
    '1': 'SHED',
    '2': 'RECONNECT'
}
freq_q = collections.deque([0]*NUM_POINTS, maxlen=NUM_POINTS)
roc_q = collections.deque([0]*NUM_POINTS, maxlen=NUM_POINTS)
ts_q = collections.deque([0]*NUM_POINTS, maxlen=NUM_POINTS)
switchStatus = "0 "*NUM_OF_SWITCHES
loadStatus = "0 "*NUM_OF_LOADS
FSMState = ""
shedLatency = 0

data_lock = threading.RLock()


def draw_figure(canvas, figure, loc=(0, 0)):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg


class ReadInput(threading.Thread):
    def run(self):
        global freq_q, roc_q, ts_q, switchStatus, loadStatus, FSMState

        for line in sys.stdin:
            if ("_fr," in line):
                keys = line.split(',')
                freq_q.append(float(keys[1]))
                roc_q.append(float(keys[2]))
                ts_q.append(float(keys[3]))
            elif("_sw," in line):
                keys = line.split(',')
                switchStatus = ' '.join(map(str, keys[1:]))
            elif("_lo," in line):
                keys = line.split(',')
                loadStatus = ' '.join(map(str, keys[1:]))
            elif("_fsm," in line):
                [_, state] = line.split(',')
                FSMState = FSMDecoder[state[0]]
            # else:
            #     print(line)


def main():
    global freq_q, roc_q, ts_q
    print("ENTERED PYTHON APP\r\n")
    bg_col = sg.theme_background_color()
    ReadInput().start()

    # define the form layout
    layout = [[sg.Canvas(size=(640, 480), key='-CANVAS-')],
              [sg.Text('', size=(25, 1), font=('Helvetica', 15),
                       justification='left', pad=((45, 0), 3), key='switchStatusText', background_color='lavender', text_color='black'), sg.Text('', size=(25, 1), font=('Helvetica', 15),
                       justification='left', key='loadStatusText', background_color='lavender', text_color='black')],
              [sg.Text('', size=(25, 1), font=('Helvetica', 15),
                       justification='left', pad=((45, 0), 3), key='FSMStateText', background_color='lavender', text_color='black'), sg.Text('', size=(25, 1), font=('Helvetica', 15),
                       justification='left', key='reactionTimeText', background_color='lavender', text_color='black')]]

    # create the form and show it without the plot
    window = sg.Window('CS723 Visualiser',
                       layout, finalize=True)

    canvas_elem = window['-CANVAS-']
    canvas = canvas_elem.TKCanvas
    switchTextBox = window['switchStatusText']
    loadTextBox = window['loadStatusText']
    FSMTextBox = window['FSMStateText']
    reactionTextBox = window['reactionTimeText']
    # draw the initial plot in the window
    fig = Figure(facecolor=bg_col)

    ax = fig.add_subplot(111)
    ax.set_xlabel("Time (ticks)")
    ax.set_ylabel("Freq (Hz)")
    ax.set_ylim([46.0, 52.0])
    ax.grid()
    fig.set_tight_layout(True)
    fig_agg = draw_figure(canvas, fig)

    while(True):
        event, values = window.read(timeout=10)
        if event in ('Exit', None):
            exit(69)
        ax.cla()                    # clear the subplot
        ax.grid()                   # draw the grid
        ax.plot(range(NUM_POINTS), list(freq_q),  color='darkblue')
        ax.set_facecolor(bg_col)
        ax.set_xlabel("Time (ticks)")
        ax.set_ylabel("Freq (Hz)")
        ax.set_ylim([46.0, 52.0])
        ax.set_xlim(0, NUM_POINTS)
        fig_agg.draw()
        # Update switches
        switchTextBox.update(f'Switches:    {switchStatus}')
        # Update loads
        loadTextBox.update(f'Loads:        {loadStatus}')
        # Update FSM
        FSMTextBox.update(f'FSM State: {FSMState}')
        reactionTextBox.update(f'Shed Latency:             {shedLatency:>04}ms')

    window.close()


if __name__ == "__main__":
    main()
