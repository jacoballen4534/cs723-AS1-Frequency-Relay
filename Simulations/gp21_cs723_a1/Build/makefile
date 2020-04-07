################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../msakefile.init

RM := rm -rf

# Setup paths
REPO_BASE_PATH = ../../..
SIMULATOR_WORKSPACE_NAME = Simulations/gp21_cs723_a1
BUILD_FOLDER_NAME = Build
SIMULATOR_PROJECT_PATH = $(REPO_BASE_PATH)/$(SIMULATOR_WORKSPACE_NAME)
BUILD_FOLDER_PATH = $(SIMULATOR_PROJECT_PATH)/$(BUILD_FOLDER_NAME)

# Setup path to shared files
SOURCE_FOLDER_NAME = shared
SOURCE_FOLDER_PATH = $(REPO_BASE_PATH)/$(SOURCE_FOLDER_NAME)/

# Define flags
# Pre-processor
PRE_PROCESSOR_FLAGS = -D__SIMULATION__
# Includes
INCLUDE_FLAGS = -I$(SOURCE_FOLDER_PATH) -I$(SIMULATOR_PROJECT_PATH)/mock -I$(SIMULATOR_PROJECT_PATH)/config -I$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/include -I$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/portable/MSVC-MingW
# compiler
COMPILER_FLAGS = -O3 -Wall -c -fmessage-length=0 -MMD -MP


# All of the sources participating in the build are defined here
INCLUDE_PATHS = $(foreach path, $(SUBDIRS), $(BUILD_FOLDER_PATH)/$(path)/subdir.mk )

-include $(BUILD_FOLDER_PATH)/sources.mk
-include $(INCLUDE_PATHS)
-include $(BUILD_FOLDER_PATH)/subdir.mk
-include $(BUILD_FOLDER_PATH)/objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include $(SIMULATOR_PROJECT_PATH)/makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
# All Target
all: gp21_cs723_a1_simulator.exe

# Tool invocations
gp21_cs723_a1_simulator.exe: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MinGW C Linker'
	gcc  -o "gp21_cs723_a1_simulator.exe" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) $(BUILD_FOLDER_PATH)/gp21_cs723_a1_simulator.exe
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include $(SIMULATOR_PROJECT_PATH)/makefile.targets
