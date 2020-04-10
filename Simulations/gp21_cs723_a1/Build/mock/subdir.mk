################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
$(SIMULATOR_PROJECT_PATH)/mock/mockIO.c \
$(SIMULATOR_PROJECT_PATH)/mock/mockKeyboard.c \
$(SIMULATOR_PROJECT_PATH)/mock/mockfrequencyRelay.c \
$(SIMULATOR_PROJECT_PATH)/mock/mockSwitches.c \
$(SIMULATOR_PROJECT_PATH)/mock/mockButtons.c 

OBJS += \
$(BUILD_FOLDER_PATH)/mock/mockIO.o \
$(BUILD_FOLDER_PATH)/mock/mockKeyboard.o \
$(BUILD_FOLDER_PATH)/mock/mockfrequencyRelay.o \
$(BUILD_FOLDER_PATH)/mock/mockSwitches.o \
$(BUILD_FOLDER_PATH)/mock/mockButtons.o 

C_DEPS += \
$(BUILD_FOLDER_PATH)/mock/mockIO.d \
$(BUILD_FOLDER_PATH)/mock/mockKeyboard.d \
$(BUILD_FOLDER_PATH)/mock/mockfrequencyRelay.d \
$(BUILD_FOLDER_PATH)/mock/mockSwitches.d \
$(BUILD_FOLDER_PATH)/mock/mockButtons.d

# Each subdirectory must supply rules for building sources it contributes
$(BUILD_FOLDER_PATH)/mock/%.o: $(SIMULATOR_PROJECT_PATH)/mock/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc $(PRE_PROCESSOR_FLAGS) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


