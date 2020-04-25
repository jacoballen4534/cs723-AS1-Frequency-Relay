from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, FigureCanvasAgg
import sys
import PySimpleGUI as sg
import threading
import time
import copy
import collections
import os
import warnings
warnings.filterwarnings("ignore")


NUM_POINTS = 1000
NUM_ROC_POINTS = 100
NUM_OF_SWITCHES = 8
NUM_OF_LOADS = 8

quitRequested = False

FSMDecoder = {
    '0': 'IDLE',
    '1': 'SHED',
    '2': 'RECONNECT'
}
freq_q = collections.deque([0]*NUM_POINTS, maxlen=NUM_POINTS)
roc_q = collections.deque([0]*NUM_POINTS, maxlen=NUM_ROC_POINTS)
ts_q = collections.deque([0]*NUM_POINTS, maxlen=NUM_POINTS)
switchStatus = "0 "*NUM_OF_SWITCHES
loadStatus = "0 "*NUM_OF_LOADS
FSMState = ""
currentShedLatency = 0
averageShedLatency = 0.00  # TODO: Implament.
minShedLatency = 0
maxShedLatency = 0
frequencyThreshold = 0
rocThreshold = 0
totalRunTime = 0

data_lock = threading.RLock()


def draw_figure(canvas, figure, loc=(0, 0)):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg


class ReadInput(threading.Thread):
    def run(self):
        global freq_q, roc_q, ts_q, switchStatus, loadStatus, FSMState, quitRequested, frequencyThreshold, rocThreshold, currentShedLatency, minShedLatency, maxShedLatency
        try:
            for line in sys.stdin:
                if ("_f," in line):
                    keys = line.split(',')
                    freq_q.append(float(keys[1]))
                    roc_q.append(float(keys[2]))
                    ts_q.append(float(keys[3]))
                elif("_s," in line):
                    keys = line.split(',')
                    switchStatus = ' '.join(map(str, keys[1:]))
                elif("_l," in line):
                    keys = line.split(',')
                    loadStatus = ' '.join(map(str, keys[1:]))
                elif("_fsm," in line):
                    [_, state] = line.split(',')
                    FSMState = FSMDecoder[state[0]]
                elif("_q" in line):
                    quitRequested = True
                elif("_fth," in line):
                    [_, key] = line.split(',')
                    frequencyThreshold = float(key)
                elif("_r," in line):
                    [_, key] = line.split(',')
                    rocThreshold = float(key)
                elif("_lt," in line):
                    keys = line.split(',')
                    currentShedLatency = float(keys[1])
                    minShedLatency = float(keys[2])
                    maxShedLatency = float(keys[3])
                elif("[2J" in line):
                    [_, lcdDisplay] = line.split("J")
                    print("LCD - " + lcdDisplay)
                # elif(len(line) > 1):
                #     print(line)
        except ValueError as e:
            sys.exit()


def main():
    global freq_q, roc_q, ts_q
    print("ENTERED PYTHON APP\r\n")
    bg_col = sg.theme_background_color()
    t = ReadInput()
    #t.setDaemon(True)
    t.start()

    text_layout = [
        [sg.Text('', size=(25, 1), font=('System', 16),
                justification='left', key='switchStatusText', background_color='gray', text_color='black'), sg.Text('', pad=((45,0),3),size=(25, 1), font=('System', 16),
                                                                                                                                            justification='left', key='loadStatusText', background_color='gray', text_color='black')],
        [sg.Text('', size=(25, 1), font=('System', 16),
                justification='left', key='FSMStateText', background_color='gray', text_color='black'), sg.Text('', pad=((45,0),3),size=(25, 1), font=('System', 16),
                                                                                                                                        justification='left', key='totalUpTimeText', background_color='gray', text_color='black')],
        [sg.Text('', size=(25, 1), font=('System', 16),
                justification='left', key='currentReactionTime', background_color='gray', text_color='black'), sg.Text('', pad=((45,0),3),size=(25, 1), font=('System', 16),
                                                                                                                                            justification='left', key='averageReactionTimeText', background_color='gray', text_color='black')],
        [sg.Text('', size=(25, 1), font=('System', 16),
                justification='left', key='minReactionTime', background_color='gray', text_color='black'), sg.Text('', pad=((45,0),3),size=(25, 1), font=('System', 16),
                                                                                                                                   justification='left', key='maxReactionTimeText', background_color='gray', text_color='black')],
    ]

    # define the form layout
    layout = [[sg.Canvas(size=(640, 480), key='-CANVAS-')],
                [sg.Frame('', text_layout, font='Any 12', background_color="dim gray")],
                [sg.Button('Exit', size=(8, 1), font=('System', 15))]]

    # create the form and show it without the plot
    window = sg.Window('CS723 Visualiser',
                       layout, finalize=True, disable_close=True, element_justification="center")

    canvas_elem = window['-CANVAS-']
    canvas = canvas_elem.TKCanvas
    switchTextBox = window['switchStatusText']
    loadTextBox = window['loadStatusText']
    FSMTextBox = window['FSMStateText']
    totalUpTimeTextBox = window['totalUpTimeText']
    currentReactionTextBox = window['currentReactionTime']
    averageReactionTextBox = window['averageReactionTimeText']
    minReactionTextBox = window['minReactionTime']
    maxReactionTextBox = window['maxReactionTimeText']
    # draw the initial plot in the window
    fig = Figure(facecolor=bg_col)
    fig.set_size_inches((10, 4))
    ax = fig.add_subplot(121)
    ax.set_xlabel("Time (ticks)", family='monospace')
    ax.set_ylabel("Freq (Hz)", family='monospace')
    ax.set_ylim([46.0, 52.0])
    ax.grid()

    roc = fig.add_subplot(122)
    roc.set_xlabel("Time (ticks)", family='monospace')
    roc.set_ylabel("RoC", family='monospace')
    roc.set_ylim([-30.0, 30.0])
    roc.set_xlim(0, NUM_ROC_POINTS)
    roc.grid()

    fig.set_tight_layout(True)
    fig_agg = draw_figure(canvas, fig)

    while(True):
        event, values = window.read(timeout=10)
        if quitRequested:
            window.close()
            exit(69)
        if event in ('Exit', None):
            window.close()
            exit(69)

        ax.cla()                    # clear the subplot
        ax.grid()                   # draw the grid
        ax.plot(range(NUM_POINTS), list(freq_q),  color='darkblue')
        ax.set_facecolor(bg_col)
        ax.set_xlabel("Time (ticks)")
        ax.set_ylabel("Freq (Hz)")
        ax.set_ylim([46.0, 52.0])
        ax.set_xlim(0, NUM_POINTS)
        ax.annotate("%.2f" % freq_q[-1], (NUM_POINTS-1,
                                        freq_q[-1]), xytext=(NUM_POINTS + 6, freq_q[-1]), family='monospace')
        if (freq_q[-1] > frequencyThreshold):
            ax.axhline(y=frequencyThreshold, color='g', linestyle='-')
        else:
            ax.axhline(y=frequencyThreshold, color='r', linestyle='-')


        roc.cla()                    # clear the subplot
        roc.grid()                   # draw the grid
        roc.plot(range(NUM_ROC_POINTS), list(roc_q), color='darkblue')
        roc.set_facecolor(bg_col)
        roc.set_xlabel("Time (ticks)")
        roc.set_ylabel("RoC")
        roc.set_ylim([-30.0, 30.0])
        roc.set_xlim(0, NUM_ROC_POINTS)
        if(roc_q[-1] >= 0.0):
            roc.annotate("+%.2f" % roc_q[-1], (NUM_ROC_POINTS-1,
                                          roc_q[-1]), xytext=(NUM_ROC_POINTS + 2, roc_q[-1]), family='monospace')
        else:
            roc.annotate("%.2f" % roc_q[-1], (NUM_ROC_POINTS-1,
                                          roc_q[-1]), xytext=(NUM_ROC_POINTS + 2, roc_q[-1]), family='monospace')

        if (roc_q[-1] < rocThreshold):
            roc.axhline(y=rocThreshold, color='g', linestyle='-')
        else:
            roc.axhline(y=rocThreshold, color='r', linestyle='-')

        if (roc_q[-1] > -rocThreshold):
            roc.axhline(y=-rocThreshold, color='g', linestyle='-')
        else:
            roc.axhline(y=-rocThreshold, color='r', linestyle='-')

        fig_agg.draw()
        # Update switches
        switchTextBox.update(f'Switches:    {switchStatus}')
        # Update loads
        loadTextBox.update(f'Loads:        {loadStatus}')
        # Update FSM
        FSMTextBox.update(f'FSM State: {FSMState}')
        totalUpTimeTextBox.update(f'Total uptime(ms):  {ts_q[-1]}')
        currentReactionTextBox.update(
            f'Current Shed Latency:  {currentShedLatency:>04}ms')
        averageReactionTextBox.update(
            f'Ave Shed Latency:       {averageShedLatency:>04}ms')
        minReactionTextBox.update(
            f'Min Shed Latency:        {minShedLatency:>04}ms')
        maxReactionTextBox.update(
            f'Max Shed Latency:      {maxShedLatency:>04}ms')

    window.close()

if __name__ == "__main__":
    main()
