## Assumptions Made
 - The brief states that while the relay is managing a load, loads can be manually turned off but not back on. 
   We assume that enabling the switch during management will still allow the relay to manage that load during this reconnect cycle.
   If the relay has already skipped this load and proceeded to re-enabling lower priority loads, the relay will return to re-enable this load after the next 500ms tick due to the higher priority.
 - When disabling loads, the switches take precedence over the relay. Loads disabled manually will not be reflected in the green LED output representing management, even if the switch is disabled after the load has been managed. (If the switch is re-enabled, it will again be considered managed by the load)

##FIXME: Following setup instructions are outdated. I think it's appopriate that we have a few .bat files the markers can choose from to click rather than have them touch a console at all.

# Setting Up The Files

1. Create a folder for the source files to be located.  
   The path to this folder will be referred to as <path/to/repo>

2. Clone the repo into <path/to/repo>

# Prerequisites

## Simulator

- MinGW (tested with version 8.2.0)

## Real Device

- Quartus II 13.0 (Full, Web or Lite edition)

- Nios II 13.0 Command Shell (This should come with Quartus II 13.0 by default)

- DE2-115 Development Board

# Running The Simulator
1. Open a terminal (tested using GitBash, do not use the nios command shell)

2. Navigate to <path/to/repo>/cs723-AS1 directory.

3. Run the command `make simulator` to build the simulator executable.

4. Locate the simulator executable at:  
   <path/to/repo>/cs723-AS1/Simulator/gp21_cs723_a1/Build/gp21_cs723_a1.exe

5. Double click the executable to run it.

6. (Optional) Run `$ make simulator_clean` to remove the generated object and dependency files.

# Running on the DE2-115 Board

### Setup the DE2 Board

1. Open Quartus II 13.0

2. Open the Programmer tab

3. Add freq_relay_controller.sof, found at <path/to/repo>/cs723-AS1/Quartus

4. Program the .sof onto the board

### Build and Load the Project

NOTE: This build does not require eclipse or an eclipse project
NOTE: This is a separate makefile from the simulator above

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
