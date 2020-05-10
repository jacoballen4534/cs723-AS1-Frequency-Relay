## Assumptions Made

- The brief states that while the relay is managing a load, loads can be manually turned off but not back on.
  We assume that enabling the switch during management will still allow the relay to manage that load during this reconnect cycle.
  If the relay has already skipped this load and proceeded to re-enabling lower priority loads, the relay will return to re-enable this load after the next 500ms tick due to the higher priority.
- When disabling loads, the switches take precedence over the relay. Loads disabled manually will not be reflected in the green LED output representing management, even if the switch is disabled after the load has been managed. (If the switch is re-enabled, it will again be considered managed by the load)
- Maintenance mode will only be entered when the load shedder is idle to prevent complications relating to unknown state

# Setting Up The Files

1. Create a folder for the source files to be located.  
   The path to this folder will be referred to as <path/to/repo>

2. Place the root of the project (cs723-AS1) in <path/to/repo>

# Prerequisites

## Simulator

- MinGW (tested with version 8.2.0)

## Real Device

- Quartus II 13.0 (Full, Web or Lite edition)

- Nios II 13.0 Command Shell (This should come with Quartus II 13.0 by default)

- DE2-115 Development Board

- `$SOPC_KIT_NIOS2` must be a Windows System Variable.  
  This can be checked in Control Panel > Search 'edit the system environment variables' > Environment Variables.  
  `$SOPC_KIT_NIOS2` should be the path to nios2. An example of this is `C:\altera\13.0\nios2eds` and will change slightly depending on the version of nios installed.

# Running The Simulator

Due to a variety in computer set ups, there are 3 options for running the simulator with a decreasing amounts of automation. If one does not work, please try the next option.

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

4. Run the command `./gp21_cs723_a1_simulator.exe | py ../../../App/display_app.py` to run the frequency analyser and connect it to the display app.

5. (Optional) Run `$ make clean` to remove the generated object and dependency files.

# Running on the DE2-115 Board

### Setup the DE2 Board

1. Open Quartus II 13.0

2. Open the Programmer tab

3. Add freq_relay_controller.sof, found at <path/to/repo>/cs723-AS1/Quartus

4. Program the .sof onto the board

### Build and Load the Project

NOTE: This build does not require eclipse or an eclipse project

Like the simulator, there are 3 options to run this project. If one does not work, please try the next option.

### First Option

Open navigate to <path/to/repo>/cs723-AS1 and double click on `run_nios.bat`

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

5. (Optional) To view the output, you can view the STDOUT by running `nios2-terminal` from the command shell
   - TODO: Pipe into python app, `nios2-terminal | python3 app.py`

# Common Simulator Issues

- `c:/mingw/bin/../lib/gcc/mingw32/8.2.0/../../../../mingw32/bin/ld.exe: cannot open output file gp21_cs723_a1.exe: Permission denied`

  - This is likely due to an instance of the executable already running.
  - Open task manager and kill the process called `gp21_cs723_a1`

- The makefile for either the simulator or the nios build will fail due to being unable to find some files.
  - It is important that you only use cmd or git bash for the simulator build, and only use nios ii command shell for the nios build
  - These two terminals have different PATH variables / execute from different places, so will fail to find source files
