################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
$(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/portable/MemMang/heap_4.c 

OBJS += \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/portable/MemMang/heap_4.o 

C_DEPS += \
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
$(BUILD_FOLDER_PATH)/FreeRTOS_Source/portable/MemMang/%.o: $(SIMULATOR_PROJECT_PATH)/FreeRTOS_Source/portable/MemMang/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc $(PRE_PROCESSOR_FLAGS) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


