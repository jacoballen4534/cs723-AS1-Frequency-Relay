# Setting Up a New Simulator Project

1. **Open Eclipse and set workspace to destination folder.**
2. **Create a new empty C project**

   - File > New > C/C++ Project
   - C Managed Build > Next
     - Project Name = `cs723-AS1`
     - Project type = `Executable > Empty Project`
     - Toolchains = `MinGW GCC`
   - Finish

3. **Setup folder structure**

   - Add FreeRTOS_Source to the project by dragging the folder into eclipse and selecting the copy files and folders option
   - Create a folder called config and put the FreeRTOSConfig.h file into it.
   - Create a folder called src and put the entry point file into it.  
     _Note:_ All additional files should also go in the src folder.

4. **Follow the [Additional Setup](#Additional-Setup) steps and [Build](#Building-the-Project) / [Run](#Running-the-Project) instructions**

---

---

# Importing An Existing Simulator Project

1. **Open Eclipse and set workspace to <path/to/repo/cs723-AS1/Simulations>**

2. **Import the project**

   - File > Import
   - General > Existing Project into Workspace > Next
   - Select Root Directory
     - Browse > gp21_cs723_a1
     - Finish

3. **Follow the [Additional Setup](#Additional-Setup) steps and [Build](#Building-the-Project) / [Run](#Running-the-Project) instructions**

---

---

# Additional Setup

In order for Eclipse to resolve all imports the new directory and freeRTOS will need to be linked in Eclipse Tool settings.

1. **Open Eclipse Tool settings**

   - Right click project folder(gp21_cs723_a1) > Properties
   - C/C++ Build > Settings > Tool Settings

2. **Link the new folders**

   - GCC C Compiler > Includes > Include paths(-l) > Add
   - Add the following directory paths one at a time.

     1. `"${workspace_loc:/${ProjName}/FreeRTOS_Source/include}"`
     2. `"${workspace_loc:/${ProjName}/FreeRTOS_Source/portable/MSVC-MingW}"`
     3. `"${workspace_loc:/${ProjName}/config}"`
     4. `"${workspace_loc:/${ProjName}/mock}"`
     5. `"${workspace_loc:/${ProjName}/src}"`

3. **Link MinGW as the library**

   - MinGW C Linker > Libraries > Libraries(-l) > Add
   - add `winmm`

4. **Setup Simulator Macros**

   To allow the simulator to use the same code as the real board some preprocessor macros have been used.  
   To enable 'simulator' mode

   - Open the Eclipse tool settings.
   - GCC C Compiler > Preprocessor > Defined symbols (-D) > Add
   - add `__SIMULATION__`
   - Apply and re-build

---

---

# Building the Project

Right click the top level folder (gp21_cs723_a1) > Build Project

# Running the Project

Right click the top level folder (gp21_cs723_a1) > Run As > Local C/C++ Application.

---

---

# Using External Peripherals

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
