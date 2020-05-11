# Setting Up The Files

1. Create a folder for the source files to be located.  
   The path to this folder will be referred to as <path/to/repo>

2. Place the root of the project (cs723-AS1) in <path/to/repo>

# Prerequisites

## DE2-115 Target

- Quartus II 13.0 (Full, Web or Lite edition)

- Nios II 13.0 Command Shell (This should come with Quartus II 13.0 by default)

- DE2-115 Development Board

- `$SOPC_KIT_NIOS2` must be a Windows System Variable.  
  This can be checked in Control Panel > Search 'edit the system environment variables' > Environment Variables.  
  `$SOPC_KIT_NIOS2` should be the path to nios2. An example of this is `C:\altera\13.0\nios2eds` and will change slightly depending on the version of nios installed.

## Simulator Target

- MinGW (tested with version 8.2.0) (Should be included as part of your Nios install)

# Running on the DE2-115 Board

### Setup the DE2 Board

1. Open Quartus II 13.0

2. Open the Programmer tab

3. Add freq_relay_controller.sof, found at <path/to/repo>/cs723-AS1/Quartus

4. Program the .sof onto the board

### Build and Load the Project

NOTE: This build does not require eclipse or an eclipse project

Due to a variety in computer set-ups, there are 3 options for running the project with a decreasing amounts of automation. If one does not work, please try the next option.

### First Option

Open navigate to <path/to/repo>/cs723-AS1 and double click on `run_nios.bat`. The batch file should build the software, program it to the board, and launch the app automatically.

### Second Option

1. Open a terminal (tested using GitBash, do not use the nios command shell)

2. Navigate to <path/to/repo>/cs723-AS1 directory.

3. Run the command `./start.sh nios` to load the project onto the board and connect it to the display app.

4. (Optional) Run `$./start.sh nios clean` to remove the generated object and dependency files.

### Third Option

1. Open Nios II 13.0 Command Shell (Must be command shell, not regular terminal)

2. Navigate to <path/to/repo>/cs723-AS1/Nios/software/gp21_cs723_a1 directory.

3. Run the command `make` to build the project.

- The make might fail due to an outdated settings.bsp file. If this occurs, you should navigate to the `gp21_cs723_a1_bsp` directory and execute the following command in the Nios command shell: `nios2-bsp-generate-files --settings=settings.bsp --bsp-dir=.`

4. When the build is complete, you can load the program onto the board by executing `nios2-download -g gp21_cs723_a1.elf`

5. Open another nios II command shell and navigate to <path/to/repo>/cs723-AS1/App/Dist, and run the command `nios2-terminal | display_app.exe`

# Usage Instructions:
The frequency relay is set up for 8 loads by default. LEDR0 (right) represents the status of the lowest priority load, and LEDR7 (left) represents the highest.
LEDG0-LEDG7 display the management (shed/unshed) status of the loads.
Note that on the app, the lowest priority load is represented on the left and the highest on the right.


KEY3 can be used to enable/disable maintenance mode as long as the loads are not being automatically managed. Maintenance mode status is displayed on the app.


Keyboard input can be used to set the thresholds. The current keyboard input is displayed on the DE2's LCD, as well as the app display.
Use keyboard letters 'F' and 'R' to change modes between setting frequency and rate-of-change thresholds. This mode change should be reflected on the LCD prompt.
Type in a threshold value and hit the 'enter' key to assign it. The threshold change will be reflected on the app graph.


Be mindful not to accidentally press KEY0, as it is mapped to a Nios hard reset. 

# Running The Simulator

Similar to the real-board instructions, there are 4 options to run this project. If one does not work, please try the next option.

Additional things to note:

1. The command window must be in focus (not the display app) in order to inputs to be registered.
2. The slide switches are mapped to keys F1 - F10
3. The push buttons are mapped to keys `z,x,c,v` (z = maintenance toggle).
4. `q` will quit the simulator and close the app.
5. The LCD is shown inside the display app.
6. If the keyboard input is just getting printed on the git-bash console for options 1-3. You will need to use the Command Prompt instead (Option 4 & 5).

### First Option

Open navigate to <path/to/repo>/cs723-AS1 and double click on `run_simulator.bat`

### Second Option

1. Open a terminal (tested using GitBash, do not use the nios command shell)

2. Navigate to <path/to/repo>/cs723-AS1 directory.

3. Run the command `./start.sh simulator app` to build and launch the simulator with the display app.

4. (Optional) Run `./start.sh simulator clean` to remove the generated object and dependency files.

### Third Option

1. Open a terminal (tested using GitBash, do not use the nios command shell)

2. Navigate to <path/to/repo>/cs723-AS1/Simulations/gp21_cs723_a1/Build directory.

3. Run the command `make` to build simulator.

4. Run the command `./gp21_cs723_a1_simulator.exe | ../../../App/dist/display_app.exe` to run the frequency analyser and connect it to the display app.

5. (Optional) Run `make clean` to remove the generated object and dependency files.

### Fourth Option

Open navigate to <path/to/repo>/cs723-AS1 and double click on `run_simulator_in_cmd.bat`

### Fifth Option

NOTE: Still build the project with git-bash.

1. Open a terminal (tested using GitBash)

2. Navigate to <path/to/repo>/cs723-AS1/Simulations/gp21_cs723_a1/Build directory.

3. Run the command `make` to build simulator.

4. Open Command Prompt

5. Navigate to <path/to/repo>\cs723-AS1\Simulations\gp21_cs723_a1\Build directory.

6. In the Command Prompt window, run the command `gp21_cs723_a1_simulator.exe | ..\..\..\App\dist\display_app.exe` to run the frequency analyser and connect it to the display app.

# Common Simulator Issues

- `c:/mingw/bin/../lib/gcc/mingw32/8.2.0/../../../../mingw32/bin/ld.exe: cannot open output file gp21_cs723_a1.exe: Permission denied`

  - This is likely due to an instance of the executable already running.
  - Open task manager and kill the process called `gp21_cs723_a1`

- The makefile for either the simulator or the nios build will fail due to being unable to find some files.
  - It is important that you only use cmd or git bash for the simulator build, and only use nios ii command shell for the nios build
  - These two terminals have different PATH variables / execute from different places, so will fail to find source files
 
 # Common Nios Issues
 - JTAG can not be acquired because something else has already taken exclusive control. Can usually be fixed by unplugging and reconnecting the programmer cable.
 - Verify Failed occurs if the .sof is not flashed to the board before running the batch file.
