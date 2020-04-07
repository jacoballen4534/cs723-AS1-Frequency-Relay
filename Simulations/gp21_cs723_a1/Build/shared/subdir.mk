################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
$(SOURCE_FOLDER_PATH)/display.c \
$(SOURCE_FOLDER_PATH)/freq_handler.c \
$(SOURCE_FOLDER_PATH)/load_control.c \
$(SOURCE_FOLDER_PATH)/load_shedder.c \
$(SOURCE_FOLDER_PATH)/main.c \
$(SOURCE_FOLDER_PATH)/taskHelpers.c \
$(SOURCE_FOLDER_PATH)/user_input.c \
$(SOURCE_FOLDER_PATH)/wall_switch.c 

OBJS += \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/display.o \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/freq_handler.o \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/load_control.o \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/load_shedder.o \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/main.o \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/taskHelpers.o \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/user_input.o \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/wall_switch.o 

C_DEPS += \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/display.d \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/freq_handler.d \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/load_control.d \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/load_shedder.d \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/main.d \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/taskHelpers.d \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/user_input.d \
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/wall_switch.d 


# Each subdirectory must supply rules for building sources it contributes
$(BUILD_FOLDER_PATH)/$(SOURCE_FOLDER_NAME)/%.o: $(SOURCE_FOLDER_PATH)/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc $(PRE_PROCESSOR_FLAGS) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


