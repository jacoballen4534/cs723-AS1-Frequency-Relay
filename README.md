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

1. Open a terminal (tested using GitBash)

2. Navigate to <path/to/repo>/cs723-AS1 directory.

3. Run the command `make simulator` to build the simulator executable.

4. Locate the simulator executable at:  
   <path/to/repo>/cs723-AS1/Simulator/gp21_cs723_a1/Build/gp21_cs723_a1.exe

5. Double click the executable to run it.

6. (Optional) Run `$ make simulator_clean` to remove the generated object and dependency files.

# Running The Real Device

### Setup the DE2 Board

1. Open Quartus II 13.0

2. Open the Programmer tab

3. Add freq_relay_controller.sof, found at <path/to/repo>/cs723-AS1/Quartus

4. Program the .sof onto the board

### Build and Load the Project

1. Open Nios II 13.0 Command Shell

2. Navigate to <path/to/repo>/cs723-AS1/Nios/software/gp21_cs723_a1 directory.

3. Run the command `make` to build the project.

# Common Simulator Issues

- `c:/mingw/bin/../lib/gcc/mingw32/8.2.0/../../../../mingw32/bin/ld.exe: cannot open output file gp21_cs723_a1.exe: Permission denied`
  - This is likely due to an instance of the executable already running.
  - Open task manager and kill the process called `gp21_cs723_a1`
