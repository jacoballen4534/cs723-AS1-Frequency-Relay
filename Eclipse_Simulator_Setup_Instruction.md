# Setting up a new simulator project

1. **Open Eclipse and set workspace to destination folder.**
2. **Create a new empty C project**

   - File > New > C/C++ Project
   - C Managed Build > Next
     - Project Name = cs723-AS1
     - Project type = Executable > Empty Project
     - Toolchains = MinGW GCC
   - Finish

3. **Setup folder structure**

   - Add FreeRTOS_Source to the project by dragging the folder into eclipse and selecting the copy files and folders option
   - Create a folder called config and put the FreeRTOSConfig.h file into it.
   - Create a folder called src and put the entry point file into it.  
     _Note:_ All additional files should also go in the src folder.

4. **Link Libraries and Folders**

   - Open Eclipse Tool settings:

     - Right click project folder(gp21_cs723_a1) > Properties
     - C/C++ Build > Settings > Tool Settings

   - Add the new folders:

     - GCC C Compiler > Includes > Include paths(-l) > Add
     - Use the Workspace option and add the following folders one at a time 1.
       1. FreeRTOS_Source > include
       2. FreeRTOS_Source > portable > MSVC-MingW
       3. config
       4. src

   - Link MinGW as the library.
     - MinGW C Linker > Libraries > Libraries(-l) > Add
     - add `winmm` and click Ok

5. **Build the project**

   Right click the top level folder (gp21_cs723_a1) > Build Project

6. **Run the project**

   Right click the top level folder (gp21_cs723_a1) > Run As > Local C/C++ Application.

# Importing an existing simulator project

1. **Open Eclipse and set workspace to <path/to/repo/cs723-AS1/Simulations>**

2. **Import the project:**

   - File > Import
   - General > Existing Project into Workspace > Next
   - Select Root Directory
     - Browse > gp21_cs723_a1
     - Finish

3. **Build the project:**
   Right click the top level folder (gp21_cs723_a1) > Build Project.

4. **Run the project**

   Right click the top level folder (gp21_cs723_a1) > Run As > Local C/C++ Application.

_Note_: You may need to **Link Libraries and Folders**

# Using Additional Peripherals

Due to Eclipse's default console buffering keyboard input, some additional steps will be required to use Windows native Command Prompt.

**Add an external tool:**

- Run > External Tools > External Tools Configurations...
- Double click Program to start making a new configuration
  - Name = `cmd`
  - Location = `${system_path:cmd.exe}`
  - Working Directory = `${workspace_loc:/gp21_cs723_a1}`
  - Arguments = `/C "start ${system_path:cmd.exe} /K .\Debug\gp21_cs723_a1.exe"`
- Disable Eclipse console
  - Common tab
  - Uncheck Allocate console (necessary for input)

External Tools should now be one of the options on the main page.  
If not run via: Run > External Tools > cmd

_Note_: This should open a command prompt window which the simulator runs in, this window will need to be in focus for keyboard events to be recognised.
