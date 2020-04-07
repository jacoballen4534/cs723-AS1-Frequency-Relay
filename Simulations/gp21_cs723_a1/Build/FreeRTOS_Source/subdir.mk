################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/croutine.c \
$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/event_groups.c \
$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/list.c \
$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/queue.c \
$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/tasks.c \
$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/timers.c 

OBJS += \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/croutine.o \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/event_groups.o \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/list.o \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/queue.o \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/tasks.o \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/timers.o 

C_DEPS += \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/croutine.d \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/event_groups.d \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/list.d \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/queue.d \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/tasks.d \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/%.o: $(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc $(PRE_PROCESSOR_FLAGS) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


